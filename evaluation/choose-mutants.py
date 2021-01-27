import argparse, shutil, os, random, subprocess

parser = argparse.ArgumentParser("choose-mutants.py")
parser.add_argument("mutant_sources", help="Path to the mutant sources")
parser.add_argument("target_file", help="Path to original file")
parser.add_argument("num_mutants", help="Number of mutants to choose", type=int)
parser.add_argument("output_sources", help="Path to chosen mutant sources folder")
parser.add_argument("output_diffs", help="Path to chosen mutant diffs folder")

args = parser.parse_args()

num_chosen = len(os.listdir(args.output_sources))
available = os.listdir(args.mutant_sources)
skipped = []
while num_chosen < args.num_mutants and num_chosen + len(skipped) < len(available):
    index = random.randint(0, len(available) - 1)
    current = available[index]
    if current in os.listdir(args.output_sources) or current in skipped:
        continue

    subprocess.run(f"colordiff --strip-trailing-cr -U 30 {args.target_file} {args.mutant_sources}/{current}".split(" "))
    
    if input("OK? (y/N): ") == "y":
        shutil.copy(args.mutant_sources + "/" + current, args.output_sources)
        with open(args.output_diffs + "/" + current + ".diff", "w") as diff:
            subprocess.run(f"colordiff --strip-trailing-cr -u {args.target_file} {args.mutant_sources}/{current}".split(" "), stdout=diff)
        num_chosen = num_chosen + 1
    else:
        skipped.append(current)