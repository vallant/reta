# Evaluation instructions
----

The evaluation workflow consists of the following stages:
* Generate the mutant sources
* Discard irrelevant mutants
* Choose a number of mutants randomly
* Compile the chosen mutants
* Analyze the mutants using a reference

## Requirements

    python3+ universalmutator colourdiff

## Assumed direrctory structures
Imagine having a plugin project in the following structure:

    plugin/
        - CMakeLists.txt
        - src/
            - file1.cpp
            - file2.cpp
        - build/
            - bin/
                - plugin.vst3

The evaluation scripts assume the following structure:

    mutants/
        - plugins/
        - file1/
            - src/
            - diff/
            - chosen_src/
            - chosen_diff/
            - original_file.txt
        - file2/
            - src/
            - diff/
            - chosen_src/
            - chosen_diff/
            - original_file.txt

This subfolder-structure below the `mutants/` folder must be identical for each file that should be mutated.

==Note that the paths in the example steps below will be shown with relative paths for readability. You should always use absolute paths!==

## Mutant sources generation
In this stage the goal is to obtain mutated sources based on one individual source file of the plugin.
Additionally coloured diffs will be generated that help with coarsly assessing whether mutants are suitable for further processing.

    # Generate the unchecked mutated sources
    $ python3 generate_mutants.py plugin/src/file1.cpp mutants/file1/

    $ ls mutants/file1/src
    file1.0.cpp 
    file1.1.cpp 
    file1.2.cpp
    ...
    
    $ ls mutants/file1/diff
    file1.0.cpp.diff
    file1.1.cpp.diff
    file1.2.cpp.diff
    ...

In the next step you can inspect the generated diffs and coarsely pre-select (= delete) mutants that would probably not compile or are not hit by the audio processing. If you have identified a mutant that is not relevant for the evaluation, just delete the mutant source file.

### Coverage reports
To generate coverage reports when using llvm/clang, compile the plugin using the following flags for **linking and compiling**: `-fprofile-instr-generate -fcoverage-mapping`

    # run a reta fast test
    ./reta fast ....

    # locate the default.profraw file in the current folder
    # Use llvm to parse this file into a different representation
    llvm-profdata merge -sparse default.profraw -o default.profdata

    # Print summary coverage reports on the console
    llvm-cov report path/to/plugin/binary -instr-profile=default.profdata

    # Print detailed coverage reports as html
    llvm-cov show --format=html path/to/plugin/binary -instr-profile=default.profdata > coverage.html

Notes:
* It is possible to filter the coverage report using `-ignore-filename-regex=<PATTERN>` option to `llvm-cov`.
* On MacOS you might need to use the apple-clang tools: `/Library/Developer/CommandLineTools/usr/bin/llvm-profdata` and `/Library/Developer/CommandLineTools/usr/bin/llvm-cov`.
* On MacOS the `path/to/plugin/binary` must **not** be the path to the bundle (e.g. `plugin.vst3`) but rather the binary located in the bundle, e.g. `plugin.vst3/Contents/MacOS/plugin`.
* More information: https://llvm.org/docs/CommandGuide/llvm-cov.html

## Mutant source selection
Because the mutation tool generates a huge amount of mutations, it is necessary to select a subset of mutant sources to be used in the evaluation. In order to avoid a bias in the mutant selection, in this stage the mutant sources are presented in a random manner. The diff will be shown on the console to further improve the quality of the mutants. E.g. if a mutant that would fail to compile was missed in the previous stage, the mutant could still be skipped in this stage. As a result of this stage a chosen number of mutants will be placed in the `mutants/file1/chosen_src` folder, and the corresponding diffs will be placed in the `mutants/file1/chosen_diff` folder.
E.g. to choose 50 mutants randomly for file1, use:

    python3 choose-mutants.py plugin/src/file1.cpp mutants/file1/ 50

Finally, place a file called `original_file.txt` in the mutant source folder, that contains the absolute path to the original source file:

    echo "plugin/src/file1.cpp" > mutants/file1/original_file.txt

## Mutant compilation
After having chosen the mutants for multiple files, the next step is to compile the mutants. To support different mechanisms for compilation, it is necessary to provide the  
command used for the compilation on the command line.
This command is based on `universalmutator`, which one-by-one overrides the original file in the file system using a mutated source file and then compiles the plugin anew. 

For example, if the plugin is compiled with CMake and the binary is placed in `build/bin`, you would perform the following steps:

    # export project
    mkdir plugin/build && cd plugin/build && cmake .. && cd ../..

    # Compile the mutants
    python3 compile-mutants.py mutants/ plugin/build "cmake --build ." plugin/build/bin/plugin.vst3

This will create a folder `mutants/plugins` containing all compiling mutants from all files. They will have the same filename structure as the mutant source files, so you can easily identify which mutant binary corresponds to which mutation.

    $ ls mutants/plugins
    plugin_file1_0.vst3
    plugin_file1_1.vst3
    ...
    plugin_file2_0.vst3
    plugin_file2_1.vst3
    ...

## Mutant analysis
After the mutants are compiled you different test configurations can be used for assessing which test configuration results in a good mutation score. For this you need one compiled reference plugin. Assuming the `reta` binary is in the current folder, alongside the `plugin_reference.vst3`:

    # generate 100 random tests based on the plugin_reference.vst3, save it as config.json
    python3 ./reta generate -i ./plugin_reference.vst3 -o config.json --strategy random --num 100

    # use the tests to analyze mutants
    python3 analyze_mutants ./reta ./config.json ./plugin_reference.vst3 mutants/plugins/

This will compare all mutants against the reference in 10 threads, displaying the progress and running score on the command line. At the end two text files containing plugin names will be written, `killed.txt` and `notkilled.txt`, which can be used to further investigate the structure of killed mutants. All intermediate files will be deleted at the end of the process.