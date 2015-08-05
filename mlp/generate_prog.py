#!/usr/bin/env python
import re
import os
import argparse
import logging
from numpy import *
from math import *

# Default paths
DEFAULT_NN_FN = "nnfiles/inversek2j.nn"
DEFAULT_DAT_FN = "datfiles/inversek2j.data"
DEFAULT_C_FN = "mlp.c"

# Global parameters
W_WIDTH = 16
W_DECIMAL = 8
W_TO_FIX = pow(2, W_DECIMAL)
I_WIDTH = 16
I_DECIMAL = 8
I_TO_FIX = pow(2, I_DECIMAL)

# Sigmoid specific parameters
LUT_SIZE            = 1024
LUT_BOUNDS          = 4
CUT_OFF             = 45
LUT_GRANULARITY     = float(LUT_BOUNDS)/LUT_SIZE

# Data file parameters
FILE_OFFSET         = 3         # Data file format (should not be changed)
DATA_OFFSET         = 0         # Skips the first N input/output pairs
INPUT_LINES         = 1         # Number of input lines per input set


############################################
# Conversion functions
############################################
def float_to_fix(x, w, p):
    '''Converts a float into a fix of DECIMALs p (total width w)
    '''
    integer_width = w-p-1
    if x>=pow(2, integer_width):
        x = pow(2, integer_width) - pow(2, (-1)*p)
    if x<=(-1)*(pow(2, integer_width)):
        x = (-1)*pow(2, integer_width)
    x_scaled = x*pow(2,p)+0.5
    return int(trunc(x_scaled))

activation_modes = [
    'FANN_LINEAR',
    'FANN_THRESHOLD',
    'FANN_THRESHOLD_SYMMETRIC',
    'FANN_SIGMOID',
    'FANN_SIGMOID_STEPWISE',
    'FANN_SIGMOID_SYMMETRIC',
    'FANN_SIGMOID_SYMMETRIC_STEPWISE',
    'FANN_GAUSSIAN',
    'FANN_GAUSSIAN_SYMMETRIC',
    'FANN_GAUSSIAN_STEPWISE',
    'FANN_ELLIOT',
    'FANN_ELLIOT_SYMMETRIC',
    'FANN_LINEAR_PIECE',
    'FANN_LINEAR_PIECE_SYMMETRIC',
    'FANN_SIN_SYMMETRIC',
    'FANN_COS_SYMMETRIC',
    'FANN_SIN',
    'FANN_COS'
]

############################################
# Helper functions
############################################
def mean_squared_error(alist, blist):
    assert(len(alist)==len(blist))
    se = 0
    outputsize = len(alist[0])
    for i, outputs in enumerate(alist):
        assert(len(alist[i])==len(blist[i])==outputsize)
        for j, output in enumerate(outputs):
            se += pow((alist[i][j]-blist[i][j]), 2)
    return se/(len(alist)*outputsize)

############################################
# Define activation functions here
############################################
def linear(x, s):
    '''Linear Sigmoid
    '''
    logging.debug('LINEAR(%d):' % x)
    x = trunc(x*s)
    x = float(x)/W_TO_FIX
    logging.debug('\t OUTPUT %d:' % x)
    return x

def sigmoid(x, s):
    '''Hyperbolic Tangent function (with input scaling by steepness s)
    '''
    # scale x
    x = trunc(x*s)
    # negative flag
    is_neg = 1 if x<0 else 0
    # compute LUT index based on x
    index = trunc(int(x) >> int(W_DECIMAL+I_DECIMAL+log(LUT_GRANULARITY,2)))
    logging.debug ('\tindex(x) = %d' % index)
    # return value
    y = 0
    # FIXME: in the HW design, this is never true...
    # if (index > 0-CUT_OFF and index <= CUT_OFF):
    #     logging.debug ('\tY=X')
    #     y = x>>W_DECIMAL #FIXME: remove the +1 factor
    if (not is_neg and index > LUT_SIZE):
        logging.debug ('\tY=1')
        y = I_TO_FIX-1
    elif (is_neg and index < 0-LUT_SIZE):
        logging.debug ('\tY=-1')
        y = 0-I_TO_FIX
    else:
        index = index+0.5 if is_neg else index+0.5
        index = float((index)*LUT_GRANULARITY)
        logging.debug ('\tsig(%f)' % index)
        y = tanh(index)*(I_TO_FIX)
    logging.debug ('\tOUTPUT %d' % y)
    return trunc(y)

def symmetric(x, s):
    '''Symmetric Sigmoid (with input scaling by steepness s)
    '''
    logging.debug('SYMM_SIG(%d):' % x)
    return sigmoid(x, s)

def nonsymmetric(x, s):
    '''Non-symmetric Sigmoid (with input scaling by steepness s)
    '''
    logging.debug('SIGMOID(%d):' % x)
    out_val = sigmoid(x, s)
    out_val = trunc(out_val >> 1)
    out_val += I_TO_FIX/2
    return out_val

# Activation functions
activation_function = {
    'FANN_LINEAR': linear,
    'FANN_SIGMOID': nonsymmetric,
    'FANN_SIGMOID_SYMMETRIC': symmetric
}


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
        self.weights = []       # The weights of the neural network
        self.neurons = []       # The activation function at a neuron

        self.minweight = float("inf")
        self.maxweight = float("-inf")

        # Read the nn file
        with open(nn_file, 'r') as f:

            # Read in the topology
            for line in f:
                match = re.match (r'^\s*layer_sizes=((\d+.+)*)\s*$', line)
                if match:
                    self.layers = [int(x) for x in match.group(1).split()]

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
                            w = float_to_fix(float(weight[1]), W_WIDTH, W_DECIMAL)
                            raw_weights[n].append([n,w])
                            # Update min and max weights
                            self.minweight = float(weight[1]) if float(weight[1]) < self.minweight else self.minweight
                            self.maxweight = float(weight[1]) if float(weight[1]) > self.maxweight else self.maxweight
                matchObj = re.match (r'neurons \(num_inputs, activation_function, activation_steepness\)=(.*)', line)
                if matchObj:
                    tmp_neurons = matchObj.group(1)
                    tmp_neurons = re.sub(r'\(', '', tmp_neurons)
                    tmp_neurons = re.sub(r'\)', ';', tmp_neurons)
                    tmp_neurons = re.split(r'; ', tmp_neurons)
                    for neuron in tmp_neurons:
                        if neuron!= '':
                            neuron = re.split(r', ', neuron)
                            activation = activation_modes[int(neuron[1])]
                            steepness = float(neuron[2])
                            raw_neuron.append([activation,steepness])

            # Process the raw weights into a list of weight matrices,
            # extract the neuron type for each layer
            n_index = 0
            for l in range(len(self.layers)-1):
                w_matrix = []
                for i in range(self.layers[l]):
                    w_vector = [x[1] for x in raw_weights[n_index+i]]
                    w_matrix.append(w_vector)
                    # for j, w in enumerate(w_vector):
                    #     w_matrix[i][j] = w[1]
                n_index = n_index + self.layers[l]
                self.neurons.append(raw_neuron[n_index])
                self.weights.append(w_matrix)

        # Print out the parameters
        print self.minweight
        print self.maxweight
        print self.layers
        print self.neurons
        for matrix in self.weights:
            print ""
            for vector in matrix:
                print vector

    def evaluate(self, dat_file, test_size):
        ''' Evaluates the HW ANN on a input dataset
        '''

        # Output values - used to compute RMSE
        precise_data = []
        approx_data = []

        with open(dat_file, 'r') as f:
            lines = f.readlines()

             # Determine the test size if unset
            if not test_size:
                test_size = ((len(lines)-FILE_OFFSET)/(1+INPUT_LINES))-DATA_OFFSET
                logging.debug('Test size set to:%s\n' + str(test_size))

            # File start and end values
            f_start = FILE_OFFSET+(INPUT_LINES+1)*DATA_OFFSET
            f_end = f_start+(INPUT_LINES+1)*test_size

            for i in range(f_start, f_end, (INPUT_LINES+1)):
                # Process inputs
                inputs = []
                for j in range(INPUT_LINES):
                    inputs += [float(x) for x in lines[i+j].split()]
                # Convert inputs to fixed-point
                inputs = [float_to_fix(x, I_WIDTH, I_DECIMAL) for x in inputs]
                mat_A = array([inputs])
                # Evaluate the neural network
                for layer, w in enumerate(self.weights):
                    # Layer input vector
                    mat_A = append(mat_A, [[I_TO_FIX]], 1)
                    # Weight Matrix
                    mat_B = array(w)
                    logging.debug('A\n' + str(mat_A))
                    logging.debug('B\n' + str(mat_B))
                    # Get the steepness and activation function
                    steepness = self.neurons[layer][1]
                    sig = vectorize(activation_function[self.neurons[layer][0]])
                    mat_A = dot(mat_A, mat_B)
                    logging.debug('AxB\n' + str(mat_A))
                    mat_A = sig(mat_A, steepness)
                    logging.debug('SIG(AXB)\n' + str(mat_A))
                approx_outputs = mat_A[0].tolist()
                approx_outputs = [float(x) / (I_TO_FIX) for x in approx_outputs]
                # Log the approximate output values for RMSE computation
                approx_data.append(approx_outputs)

                # Process outputs
                precise_outputs = [float(x) for x in lines[i+INPUT_LINES].split()]
                # Log the precise output values for RMSE computation
                precise_data.append(precise_outputs)
                logging.debug('Precise Outputs\n'+str(precise_outputs))

            # Used to compute RMSE
            mse = mean_squared_error(precise_data, approx_data)
            rmse = sqrt(mse)
            print('Info:\tRMSE on HW neural network is: %f' % rmse)


def cli():
    parser = argparse.ArgumentParser(
        description='Generates c code from a FANN .nn specification'
    )
    parser.add_argument(
        '-nn', dest='nn_fn', action='store', type=str, required=False,
        default=DEFAULT_NN_FN, help='input FANN neural network specification'
    )
    parser.add_argument(
        '-dat', dest='dat_fn', action='store', type=str, required=False,
        default=DEFAULT_DAT_FN, help='input data file (input/output pairs)'
    )
    parser.add_argument(
        '-c', dest='c_fn', action='store', type=str, required=False,
        default=None, help='output c file'
    )
    parser.add_argument(
        '-size', dest='test_size', action='store', type=str, required=False,
        default=None, help='number of input sets to test'
    )
    args = parser.parse_args()

    # Logger
    logging.basicConfig(filename='output.log',level=logging.DEBUG)

    # Parse the FANN file
    ann = ANN(args.nn_fn)
    # Evaluate the ANN
    ann.evaluate(args.dat_fn, args.test_size)

if __name__ == '__main__':
    cli()
