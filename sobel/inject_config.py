#!/usr/bin/env python
import subprocess
import tempfile
import os
import shutil
import shlex
import eval
import math
import copy

MASKMAX = 32

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

def parse_relax_config(f):
    """Parse a relaxation configuration from a file-like object.
    Generates (ident, param) tuples.
    """
    for line in f:
        line = line.strip()
        if line:
            param, ident = line.split(None, 1)
            yield ident, int(param)

def dump_relax_config(config, fname):
    """Write a relaxation configuration to a file-like object. The
    configuration should be a sequence of tuples.
    """
    with open(fname, 'w') as f:
        for conf in config:
            mode = 0
            if conf['relax']==1:
                mode = (9<<16) + (conf['himask']<<8) + conf['lomask']
            f.write(str(mode)+ ' ' + conf['insn'] + '\n')
            # print (str(mode)+ ' ' + conf['insn'])

def test_config(config):
    """Creates a temporary directory to run ACCEPT with
    the passed in config object for precision relaxation.
    """
    curdir = os.getcwd()
    dirname = os.path.basename(os.path.normpath(curdir))
    tmpdir = tempfile.mkdtemp()+'/'+dirname
    print tmpdir
    copy_directory(curdir, tmpdir)
    shell(shlex.split('make clean'), cwd=tmpdir)
    dump_relax_config(config, tmpdir+'/accept_config.txt')
    shell(shlex.split('make run_opt'), cwd=tmpdir)

    # Now that we're done with the compilation, evaluate results
    output_fp = os.path.join(tmpdir,'out.pgm')
    if os.path.isfile(output_fp):
        error = eval.score('precise.pgm',os.path.join(tmpdir,'out.pgm'))
    else:
        # Program crashed
        error = 1000000.0
    # os.removedirs(tmpdir)
    return error

def tune_himask(config, idx):
    print ("------------------------")
    print ("Tuning instruction " + str(idx))
    print ("------------------------")
    # If the instruction cannot be tuned, return 0
    if config[idx]['relax']==0:
        print "skip"
        return 0
    # Temporary configuration
    tmp_config = copy.deepcopy(config)
    # Initialize the mask and best mask variables
    maskval = MASKMAX/2
    best = 0
    # Now to the autotune part - do a log mask exploration
    for i in range(0, int(math.log(MASKMAX, 2))):
        print "testing himask of value: " + str(maskval)
        tmp_config[idx]['himask'] = maskval
        error = test_config(tmp_config)
        if error==0:
            best = maskval
            maskval += MASKMAX>>(i+2)
        else:
            maskval -= MASKMAX>>(i+2)
        print "error: " + str(error)

    return best

def adapt_config(coarse_fn, fine_fn):
    """Take a coarse (per-function) configuration file and apply it to a
    fine (per-instruction) ACCEPT configuration file. The latter is
    modified on disk.

    The first argument is the filename of a text file containing
    entries like this:

        function_name parameter

    where the parameter is an integer. There can also be a line where
    the function_name is "default", in which case all other instructions
    (outside of the listed functions) are given this parameter.

    All non-instruction parameters are left as 0.

    The second argument is the filename of an ACCEPT configuration file.
    """
    # Load the coarse configuration file.
    params = {}
    default_param = 0
    with open(coarse_fn) as f:
        for line in f:
            line = line.strip()
            if line:
                func, param = line.split()
                param = int(param)
                if func == 'default':
                    default_param = param
                else:
                    params[func] = param

    # Load ACCEPT config and adjust parameters.
    config = []
    with open(fine_fn) as f:
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

    # Now test the base config
    # test_config(config)

    # Let's tune the high mask bits (0 performance degradation)
    for idx, conf in enumerate(config):
        himask = tune_himask(config, idx)
        config[idx]['himask'] = himask

    # Now let's relax the instructions
    target_error = 0.1
    bitbudget = 1000
    rate = 1
    prev_minerror = 0.0
    for bit in range(0,bitbudget):
        print ("------------------------")
        print ("Bit tuning # " + str(bit))
        print ("------------------------")
        minerror, minidx = float("inf"), -1
        zero_error = []
        # for idx, conf in enumerate(config):
        for idx in range(0,len(config)):
            conf = config[idx]
            print ("Testing instruction " + str(idx))
            if config[idx]['relax']==0:
                print "skip"
            else:
                tmp_config = copy.deepcopy(config)
                tmp_config[idx]['lomask'] += rate
                print "testing lomask of value: " + str(tmp_config[idx]['lomask'])
                error = test_config(tmp_config)
                print "error: " + str(error)
                if error == prev_minerror:
                    # Check if we're zeroing all of the bits
                    if (tmp_config[idx]['himask']+tmp_config[idx]['lomask']) < 32:
                        zero_error.append(idx)
                elif error<minerror:
                    print "new min error!!!"
                    minerror = error
                    minidx = idx
        print zero_error
        for idx in zero_error:
            config[idx]['lomask'] += rate
            print("Increasing bitmask @ insn %d to %d" % (idx, config[idx]['lomask']))
        if minerror <= target_error:
            config[minidx]['lomask'] += rate
            prev_minerror = minerror
            print("Increasing bitmask @ insn %d to %d" % (minidx, config[minidx]['lomask']))
        elif not zero_error:
            break

    for conf in config:
        print conf

    # Dump back to the fine (ACCEPT) configuration file.
    # with open(fine_fn, 'w') as f:
    #     dump_relax_config(config, f)


if __name__ == '__main__':
    adapt_config('inject_config.txt', 'accept_config.txt')
