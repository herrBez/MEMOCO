You find here the  source code implementing an optimizer for TSP based on neighbourhood search, and two data file for two different instances (tsp12.dat and tsp60.dat).
Different versions of the optimizer can be obtained according to the following instructions.

- folder 0.skeleton: 
basic local search schema (TO BE COMPLETED with neighbouhood exploration)

- folder 1.LS: 
use the files in this folder (deleting the suffix LS) to replace the corresponding files in "folder 0.skeleton" to obtain a local search (the new file contains an implementation of the neighbouhood exploration)

- folder 2.TS: 
use the files in this folder (deleting the suffix TS) to replace the corresponding files in "folder 0.skeleton + folder 1.LS" to obtain a Tabu Search

- folder 3.TSAC:
use the files in this folder (deleting the suffix TS) to replace the corresponding files in "folder 0.skeleton + folder 1.LS + folder 2.TS" to obtain a Tabu Search with the aspiration criterion "accept a tabu move if it yields the new incumbent solution"

NOTICE (tabu list length calibration): the critical values (lengths under which the search loops) are 4-5 for tsp12.dat and 7-8 for tsp60.dat.
