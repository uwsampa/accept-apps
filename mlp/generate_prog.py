#!/usr/bin/env python
import re
import os
import argparse
import numpy
import math

# Default paths
DEFAULT_NN_FN = "nnfiles/inversek2j.nn"
DEFAULT_C_FN = "mlp.c"

# Global parameters
W_INTEGER = 8
W_PRECISION = 7


############################################
# Conversion functions
############################################
def float_to_fix(x, p):
    '''Converts a float into a fix of precisions p
    '''
    if x>=pow(2, W_INTEGER):
        x = pow(2, W_INTEGER) - pow(2, (-1)*p)
    if x<=(-1)*(pow(2, W_INTEGER)):
        x = (-1)*pow(2, W_INTEGER)
    x_scaled = x*pow(2,p)+0.5
    x_scaled = math.floor(x_scaled) #FIXME: bug with trunc
    return x_scaled

############################################
# ANN Class
############################################
class ANN:
    ''' A multi-layer perceptron class
    '''

    def __init__(self, nn_file):
        ''' Reads the ANN description from the .nn file
        '''
        # ANN description
        self.layers  = []       # A list describing the ANN's topology
        self.inputs  = 0        # Number of inputs
        self.outputs = 0        # Number of outputs
        self.weights = []       # The weights of the neural network
        self.neurons = []       # The activation function at a neuron

        # Read the nn file
        with open(nn_file, 'r') as f:

            # Read in the topology
            for line in f:
                match = re.match (r'^\s*layer_sizes=((\d+.+)*)\s*$', line)
                if match:
                    self.layers = [int(x) for x in match.group(1).split()]

            # Derive the input and output sizes
            self.inputs = self.layers[0]-1
            self.outputs = self.layers[len(self.layers)-1]-1

            # Compute the total number of neurons
            num_neurons = sum(self.layers) - self.layers[len(self.layers)-1]

            # Read in the raw weights + neuron activation functions
            raw_weights = [[] for i in range(num_neurons)]
            raw_neuron = []
            f.seek(0)
            for line in f:
                matchObj = re.match (r'connections \(connected_to_neuron, weight\)=(.*)', line)
                if matchObj:
                    tmp_weights = matchObj.group(1)
                    tmp_weights = re.sub(r'\(', '', tmp_weights)
                    tmp_weights = re.sub(r'\)', ';', tmp_weights)
                    tmp_weights = re.split(r'; ', tmp_weights)
                    for weight in tmp_weights:
                        if weight!= '':
                            weight = re.split(r', ', weight)
                            n = int(weight[0])
                            w = float_to_fix(float(weight[1]), W_PRECISION)
                            raw_weights[n].append([n,w])
                matchObj = re.match (r'neurons \(num_inputs, activation_function, activation_steepness\)=(.*)', line)
                if matchObj:
                    tmp_neurons = matchObj.group(1)
                    tmp_neurons = re.sub(r'\(', '', tmp_neurons)
                    tmp_neurons = re.sub(r'\)', ';', tmp_neurons)
                    tmp_neurons = re.split(r'; ', tmp_neurons)
                    for neuron in tmp_neurons:
                        if neuron!= '':
                            neuron = re.split(r', ', neuron)
                            activation = int(neuron[1])
                            steepness = float(neuron[2])
                            raw_neuron.append([activation,steepness])

            # Process the raw weights into a list of weight matrices,
            # extract the neuron type for each layer
            n_index = 0
            for l in range(len(self.layers)-1):
                w_matrix = numpy.zeros(shape=(self.layers[l],  self.layers[l+1]-1))
                for i in range(self.layers[l]):
                    w_vector = raw_weights[n_index+i]
                    for j, w in enumerate(w_vector):
                        w_matrix[i][j] = w[1]
                n_index = n_index + self.layers[l]
                self.neurons.append(raw_neuron[n_index])
                self.weights.append(w_matrix)

        # Print out the parameters
        print self.layers
        print self.inputs
        print self.outputs
        print self.weights
        print self.neurons

def cli():
    parser = argparse.ArgumentParser(
        description='Generates c code from a FANN .nn specification'
    )
    parser.add_argument(
        '-nn', dest='nn_fn', action='store', type=str, required=False,
        default=DEFAULT_NN_FN, help='input FANN neural network specification'
    )
    parser.add_argument(
        '-c', dest='c_fn', action='store', type=str, required=False,
        default=None, help='output c file'
    )
    args = parser.parse_args()

    # Parse the FANN file
    ann = ANN(args.nn_fn)

if __name__ == '__main__':
    cli()
