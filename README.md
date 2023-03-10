## API Project [2022]
The project delievered as the final exam of API can be found in the main directory (/main.c), meanwhile the description of project can be found in the docs section. The project tries to replicate a game like mastermind; 

<!-- ABOUT THE PROJECT -->
## API Project Verifier [2022]
This project is meant to be a substitute of the online verifier of the API project 2022, and it's still in an embryonic version. It has to be calibrated since, obviously, the execution time of the program on your machine are probably going to differ from those of the online verifier; for this reason, it is mostly useful to compile and check if the output is correct, with only a single action, and check if execution times are decreasing.
## Usage
This tool requires _massif tool_ to profile memory usage. It can be installed following instructions below:

1. Install valgrind via shell
   ```sh
    sudo apt-get install valgrind
    ```
2. Install massif GUI
   ```sh
    sudo apt-get install massif-visualizer
   ```
Inside the folder of your project, you should place few folders as explained below:
   ```sh
    \project
    main.c
      \test_suite
      ...
      test1.txt
      test2.txt
      ...
      \results
        ...
        test1.txt
        test2.txt
        ...
   ```
You should specify the project folder in the code of _main.py_. Tests and outputs must have the same name. 
The number of iterations of tests can be choosen, default is 100. 
