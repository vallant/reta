import argparse, shutil, os, random, subprocess

parser = argparse.ArgumentParser("choose-mutants.py")
parser.add_argument("target_file", help="Path to original file")
parser.add_argument("mutant_sources", help="Path to the mutant sources")
parser.add_argument("num_mutants", help="Number of mutants to choose", type=int)

args = parser.parse_args()


def get_random_mutant():
    index = random.randint(0, len(available_mutants) - 1) 
    return available_mutants[index] 


def get_next_mutant():
    mutant = get_random_mutant()
    # randomly choose mutants until we found one that
    # * wasn't already chosen
    # * wasn't skipped before
    while mutant in os.listdir(args.output_sources) or mutant in skipped_mutants:
        mutant = get_random_mutant()
    return mutant

def generate_diff(reference, actual, file = None):
    cmd = f"colordiff --strip-trailing-cr -U 15 {reference} {actual}"
    if file == None:
        subprocess.run(cmd.split(" "))
    else:
        subprocess.run(cmd.split(" "), stdout=file)

chosen_mutants = os.listdir(args.output_sources)
available_mutants = os.listdir(args.mutant_sources)
skipped_mutants = []

chosen_mutants_src_folder = os.path.dirname(args.mutant_sources) + "/chosen_src"
chosen_mutants_diff_folder = os.path.dirname(args.mutant_sources) + "/chosen_diff"

# as long as we didn't hit the target AND there are still mutants left
while len(chosen_mutants) < args.num_mutants and len(chosen_mutants) + len(skipped_mutants) < len(available_mutants):

    current_mutant = get_next_mutant()

    # show the diff
    generate_diff(args.target_file, args.mutant_sources + "/" + current_mutant)

    # wait for user input whether the mutant should be chosen
    if input("OK? (y/N): ") == "y":

        # copy the mutant to its destination
        shutil.copy(args.mutant_sources + "/" + current_mutant, chosen_mutants_src_folder)
        
        # generate the diff and save it as a file
        with open(chosen_mutants_diff_folder + "/" + current_mutant + ".diff", "w") as file:
            generate_diff(args.target_file, args.mutant_sources + "/" + current_mutant, file)

        chosen_mutants.append(current_mutant)
    else:
        skipped_mutants.append(current_mutant)