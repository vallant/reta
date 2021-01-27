import argparse, subprocess, os

parser = argparse.ArgumentParser("generate-mutants.py")
parser.add_argument("target_file", help="Path to file to mutate")
parser.add_argument("mutations_folder", help="Path to folder for mutated source files")
parser.add_argument("diff_folder", help="Path to folder for diffs")

args = parser.parse_args()

subprocess.run(f"mutate {args.target_file} cpp --mutantDir {args.mutations_folder} --noFastCheck".split(" "))

mutants = os.listdir(args.mutations_folder)
for mutant in mutants:
    mutant_path = args.mutations_folder + "/" + mutant

    with open(args.diff_folder + "/" + mutant + ".diff", "w") as file:
        subprocess.run(f"diff --strip-trailing-cr -u {args.target_file} {mutant_path}".split(" "), stdout=file)