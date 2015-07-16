#!/usr/bin/env python
import subprocess
import tempfile
import os
import shutil
import shlex
import eval
import math
import copy
import argparse
import logging
import cw.client
import threading
import collections

# FILE NAMES
ACCEPT_CONFIG = 'accept_config.txt'
INJECT_CONFIG = 'inject_config.txt'
LOG_FILE = 'inject_config.log'
PRECISE_OUTPUT = 'orig.pgm'
APPROX_OUTPUT = 'out.pgm'

# PARAMETERS
DATA_WIDTH = 32
MASK_MAX = DATA_WIDTH

#################################################
# General OS function helpers
#################################################

def shell(command, cwd=None, shell=False):
    """Execute a command (via a shell or directly). Capture the stdout
    and stderr streams as a string.
    """
    return subprocess.check_output(
        command,
        cwd=cwd,
        stderr=subprocess.STDOUT,
        shell=shell,
    )

# Taken from Python Central
def copy_directory(src, dest):
    """Copies a directory recursively, and does
    error handling.
    """
    try:
        shutil.copytree(src, dest)
    # Directories are the same
    except shutil.Error as e:
        print('Directory not copied. Error: %s' % e)
    # Any error saying that the directory doesn't exist
    except OSError as e:
        print('Directory not copied. Error: %s' % e)

#################################################
# Configuration file reading/processing
#################################################

def parse_relax_config(f):
    """Parse a relaxation configuration from a file-like object.
    Generates (ident, param) tuples.
    """
    for line in f:
        line = line.strip()
        if line:
            param, ident = line.split(None, 1)
            yield ident, int(param)

def gen_default_config(inject_config_fn):
    """Reads in the coarse error injection descriptor,
    generates the default config by running make run_orig.
    Returns a config object.
    """
    curdir = os.getcwd()

    # Load the coarse configuration
    logging.info('Reading in the coarse config file: {}'.format(inject_config_fn))
    params = {}
    default_param = 0
    with open(inject_config_fn) as f:
        for line in f:
            line = line.strip()
            if line:
                func, param = line.split()
                param = int(param)
                if func == 'default':
                    default_param = param
                else:
                    params[func] = param

    logging.info('Generating the fine config file: {}'.format(ACCEPT_CONFIG))
    shell(shlex.split('make clean'), cwd=curdir)
    shell(shlex.split('make run_orig'), cwd=curdir)

    # Load ACCEPT config and adjust parameters.
    config = []
    with open(ACCEPT_CONFIG) as f:
        for ident, param in parse_relax_config(f):
            # Turn everything off except selected functions.
            param = 0

            # If this is in a function indicated in the parameters file,
            # adjust the parameter accordingly.
            if ident.startswith('instruction'):
                _, i_ident = ident.split()
                func, _, _ = i_ident.split(':')
                if func in params:
                    param = params[func]
                else:
                    param = default_param

            config.append({'insn': ident, 'relax': param, 'himask': 0, 'lomask': 0})

    return config

def dump_relax_config(config, fname):
    """Write a relaxation configuration to a file-like object. The
    configuration should be a sequence of tuples.
    """
    logging.debug("-----------FILE DUMP BEGIN-----------")
    with open(fname, 'w') as f:
        for conf in config:
            mode = 0
            if conf['relax']==1:
                mode = (9<<16) + (conf['himask']<<8) + conf['lomask']
            f.write(str(mode)+ ' ' + conf['insn'] + '\n')
            logging.debug(str(mode)+ ' ' + conf['insn'])
    logging.debug("----------- FILE DUMP END -----------")

#################################################
# Configuration function
#################################################

def print_config(config):
    """Prints out the configuration.
    """
    logging.debug("-----------CONFIG DUMP BEGIN-----------")
    for conf in config:
        print conf
    logging.debug("----------- CONFIG DUMP END -----------")

def eval_compression_factor(config):
    """Evaluate number of bits saved from compression.
    """
    bits, total = 0, 0
    for conf in config:
        # Only account for the instructions that we can relax
        if (conf['relax']):
            bits += conf['himask']+conf['lomask']
            total += DATA_WIDTH
    return float(bits)/total

def test_config(config):
    """Creates a temporary directory to run ACCEPT with
    the passed in config object for precision relaxation.
    """
    # Get the current working directory
    curdir = os.getcwd()
    # Get the last level directory name (the one we're in)
    dirname = os.path.basename(os.path.normpath(curdir))
    # Create a temporary directory
    tmpdir = tempfile.mkdtemp()+'/'+dirname
    logging.debug('New directory created: {}'.format(tmpdir))
    # Transfer files over
    copy_directory(curdir, tmpdir)
    # Cleanup
    shell(shlex.split('make clean'), cwd=tmpdir)
    # Dump config
    dump_relax_config(config, tmpdir+'/'+ACCEPT_CONFIG)
    # Full compile and program run
    logging.debug('Lanching compile and run...')
    try:
        shell(shlex.split('make run_opt'), cwd=tmpdir)
    except:
        logging.info('Make error!')

    # Now that we're done with the compilation, evaluate results
    output_fp = os.path.join(tmpdir,APPROX_OUTPUT)
    if os.path.isfile(output_fp):
        error = eval.score(PRECISE_OUTPUT,os.path.join(tmpdir,APPROX_OUTPUT))
        logging.info('Reported application error: {}'.format(error))
    else:
        # Program crashed, set the error to an arbitratily high number
        logging.info('Program crashed')
        error = float('inf')
    # Remove the temporary directory
    shutil.rmtree(tmpdir)
    # Return the error
    return error

#################################################
# Parameterisation testing
#################################################

def tune_himask_insn(base_config, idx):
    """ Tunes the most significant bit masking of
    an instruction given its index without affecting
    application error.
    """
    # Generate temporary configuration
    tmp_config = copy.deepcopy(base_config)
    # Initialize the mask and best mask variables
    mask_val = MASK_MAX>>1
    best_mask = 0
    # Now to the autotune part - do a log exploration
    for i in range(0, int(math.log(MASK_MAX, 2))):
        logging.info ("Increasing himask on instruction {} to {}".format(idx, conf['insn']))
        # Set the mask in the temporary config
        tmp_config[idx]['himask'] = mask_val
        # Test the config
        error = test_config(tmp_config)
        # Check the error, and modify mask_val accordingly
        if error==0:
            logging.info ("New best mask!")
            best_mask = mask_val
            mask_val += MASK_MAX>>(i+2)
        else:
            mask_val -= MASK_MAX>>(i+2)
    # Corner case: bitmask=31, test 32
    if best_mask==MASK_MAX-1:
        mask_val = MASK_MAX
        logging.info ("Testing himask: {}".format(mask_val))
        # Set the mask in the temporary config
        tmp_config[idx]['himask'] = MASK_MAX
        # Test the config
        error = test_config(tmp_config)
        if error==0:
            logging.info ("New best mask!")
            best_mask = mask_val
    # Set the himask of that instruction
    base_config[idx]['himask'] = best_mask
    logging.info ("Himask tuned to: {}".format(best_mask))
    logging.info ("[error, savings]: [0.0, {}]\n".format(eval_compression_factor(base_config)))

def tune_himask(base_config, clusterworkers):
    """ Tunes the most significant bit masking at an instruction
    granularity without affecting application error.
    """
    logging.info ("Tuning high-order bit mask\n")
    for idx, conf in enumerate(base_config):
        logging.info ("Tuning instruction: {}".format(conf['insn']))
        # If the instruction should not be tuned, return 0
        if conf['relax']==0:
            logging.info ("Skipping current instruction\n")
        else:
            tune_himask_insn(base_config, idx)
    return best_mask

def tune_lomask(base_config, clusterworkers, target_error, passlimit, rate=1):
    """ Tunes the least significant bits masking to meet the
    specified error requirements, given a passlimit.
    The tuning algorithm performs multiple passes over every
    instructions. For each pass, it masks the LSB of each instuction
    DST register value. At the end of each pass, it masks off the
    instructions that don't affect error at all, and masks the instruction
    that affects error the least. This process is repeated until the target
    error is violated, or the passlimit is reached.
    """
    logging.info ("Tuning low-order bit mask\n")

    # Map job IDs to instruction index
    jobs = {}
    jobs_lock = threading.Lock()

    # Map instructions to errors
    insn_errors = collections.defaultdict(list)

    def completion(jobid, output):
        with jobs_lock:
            idx = jobs.pop(jobid)
        logging.info ("Bit tuning on instruction {} done!".format(idx))
        insn_errors[idx] = output

    if (clusterworkers):
        # Kill the master/workers in case previous run failed
        logging.info ("Stopping master/workers that are still running")
        cw.slurm.stop()
        # Start the workers & master
        logging.info ("Starting {} worker(s)".format(clusterworkers))
        cw.slurm.start(nworkers=clusterworkers)
        client = cw.client.ClientThread(completion, cw.slurm.master_host())
        client.start()

    # Previous min error (to keep track of instructions that don't impact error)
    prev_minerror = 0.0
    # List of instructions that are tuned optimally
    maxed_insn = []
    # Passes
    for tuning_pass in range(0, passlimit):
        logging.info ("Bit tuning pass #{}".format(tuning_pass))
        # Now iterate over all instructions
        for idx, conf in enumerate(base_config):
            logging.info ("Increasing lomask on instruction {} to {}".format(idx, conf['insn']))
            if conf['relax']==0:
                insn_errors[idx] = float('inf')
                logging.info ("Skipping current instruction {} - relaxation disallowed".format(idx))
            elif idx in maxed_insn:
                insn_errors[idx] = float('inf')
                logging.info ("Skipping current instruction {} - will degrade quality too much".format(idx))
            elif (base_config[idx]['himask']+base_config[idx]['lomask']) == 32:
                insn_errors[idx] = float('inf')
                logging.info ("Skipping current instruction {} - bitmask max reached".format(idx))
            else:
                # Generate temporary configuration
                tmp_config = copy.deepcopy(base_config)
                # Increment the LSB mask value
                tmp_config[idx]['lomask'] += rate
                logging.info ("Testing lomask of value {} on instruction {}".format(tmp_config[idx]['lomask'], idx))
                # Test the config
                if (clusterworkers):
                    jobid = cw.randid()
                    with jobs_lock:
                        jobs[jobid] = idx
                    client.submit(jobid, test_config, tmp_config)
                else:
                    error = test_config(tmp_config)
                    insn_errors[idx] = error
        if (clusterworkers):
            logging.info('All jobs submitted for pass #{}'.format(tuning_pass))
            client.wait()
            logging.info('All jobs finished for pass #{}'.format(tuning_pass))

        ###################
        # Post Processing #
        ###################
        logging.debug ("Errors: {}".format(insn_errors))

        # Keep track of the instruction that results in the least postive error
        minerror, minidx = float("inf"), -1
        # Keep track of the instructions that results zero error
        zero_error = []
        for idx, conf in enumerate(base_config):
            error = insn_errors[idx]
            # Update min error accordingly
            if error == prev_minerror:
                zero_error.append(idx)
            elif error<minerror:
                minerror = error
                minidx = idx
            else:
                # The error is too large, so let's tell the autotuner
                # not to revisit this instruction during later passes
                maxed_insn.append(idx)
        # Apply LSB masking to the instruction that are not impacted by it
        logging.debug ("Zero-error instruction list: {}".format(zero_error))
        for idx in zero_error:
            base_config[idx]['lomask'] += rate
            logging.info ("Increasing lomask on instruction {} to {}".format(idx, tmp_config[idx]['lomask']))
        # Apply LSB masking to the instruction that minimizes positive error
        logging.debug ("[minerror, target_error] = [{}, {}]".format(minerror, target_error))
        if minerror <= target_error:
            base_config[minidx]['lomask'] += rate
            prev_minerror = minerror
            logging.info ("Increasing lomask on instruction {} to {}".format(minidx, tmp_config[minidx]['lomask']))
            logging.info ("[error, savings]: [{}, {}]\n".format(minerror, eval_compression_factor(base_config)))
        # Empty list
        elif not zero_error:
            break
        logging.info ("Bit tuning pass #{} done!\n".format(tuning_pass))

    if(clusterworkers):
        cw.slurm.stop()

#################################################
# Main Function
#################################################

def tune_width(inject_config_fn, clusterworkers, target_error, passlimit):
    """Performs instruction masking tuning
    """
    # Generate default configuration
    config = gen_default_config(inject_config_fn)

    # Let's tune the high mask bits (0 performance degradation)
    tune_himask(config, clusterworkers)

    # Now let's tune the low mask bits (performance degradation allowed)
    tune_lomask(config, clusterworkers, target_error, passlimit)

    # Dump back to the fine (ACCEPT) configuration file.
    dump_relax_config(config, ACCEPT_CONFIG)

    # Print the final conf object
    print_config(config)

#################################################
# Argument validation
#################################################

def cli():
    parser = argparse.ArgumentParser(
        description='Bit-width tuning using masking'
    )
    parser.add_argument(
        '-f', dest='inject_config_fn', action='store', type=str, required=False,
        default=INJECT_CONFIG, help='error injection configuration file'
    )
    parser.add_argument(
        '-t', dest='target_error', action='store', type=float, required=False,
        default=0.1, help='target application error'
    )
    parser.add_argument(
        '-p', dest='passlimit', action='store', type=int, required=False,
        default=1000, help='maximum number of passes when masking off LSBs'
    )
    parser.add_argument(
        '-c', dest='clusterworkers', action='store', type=int, required=False,
        default=0, help='parallelize on cluster'
    )
    parser.add_argument(
        '-d', dest='debug', action='store_true', required=False,
        default=False, help='print out debug messages'
    )
    parser.add_argument(
        '-log', dest='logpath', action='store', type=str, required=False,
        default=LOG_FILE, help='path to log file'
    )
    args = parser.parse_args()

    # Take care of logger
    logFormatter = logging.Formatter("%(asctime)s [%(threadName)-12.12s] [%(levelname)-5.5s]  %(message)s", datefmt='%m/%d/%Y %I:%M:%S %p')
    rootLogger = logging.getLogger()

    fileHandler = logging.FileHandler(args.logpath)
    fileHandler.setFormatter(logFormatter)
    rootLogger.addHandler(fileHandler)

    consoleHandler = logging.StreamHandler()
    consoleHandler.setFormatter(logFormatter)
    rootLogger.addHandler(consoleHandler)

    if(args.debug):
        rootLogger.setLevel(logging.DEBUG)
    else:
        rootLogger.setLevel(logging.INFO)

    tune_width(args.inject_config_fn, args.clusterworkers, args.target_error, args.passlimit)

if __name__ == '__main__':
    cli()
