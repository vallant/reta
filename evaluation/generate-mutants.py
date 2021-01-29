import argparse, subprocess, os

parser = argparse.ArgumentParser("generate-mutants.py")
parser.add_argument("target_file", help="Path to file to mutate")
parser.add_argument("base_folder", help="Path to base folder for the mutation of this file")

args = parser.parse_args()

src_folder = args.base_folder + "/src"
diff_folder = args.base_folder + "/diff"

subprocess.run(f"mutate {args.target_file} cpp --mutantDir {src_folder} --noFastCheck".split(" "))

mutants = os.listdir(src_folder)
for mutant in mutants:
    mutant_path = src_folder + "/" + mutant

    with open(diff_folder + "/" + mutant + ".diff", "w") as file:
        subprocess.run(f"diff --strip-trailing-cr -u {args.target_file} {mutant_path}".split(" "), stdout=file)