import argparse, subprocess, os
parser = argparse.ArgumentParser("compile-mutants")
parser.add_argument("base_folder", help="Path to folder containing individual mutant file folders")
parser.add_argument("plugin_name", help="Plugin name to compile")
args = parser.parse_args()

for mutant_folder in os.listdir(args.base_folder):
    if "plugins" in mutant_folder or "juce_Gain" in mutant_folder:
        continue
    mutant_folder = args.base_folder + "/" + mutant_folder
    with open(mutant_folder + "/original_file.txt") as f:    
        original_file = f.readline().strip("\n")
    
    command = []
    command += ["analyze_mutants", original_file]
    command += [f"python3 compile-mutant.py {mutant_folder}/chosen_src {args.base_folder}/plugins {args.plugin_name} {original_file}"]
    command += ["--noShuffle", "--verbose", "--timeout", "120"]
    command += ["--mutantDir", mutant_folder + "/chosen_src"]

    try:
        subprocess.run(command)
    except Exception as e:
        print(e)