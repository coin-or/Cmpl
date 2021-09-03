# CMPL

CMPL (<Coliop|Coin> Mathematical Programming Language) is a mathematical programming language and a system for mathematical programming and optimisation of linear optimisation problems.
The CMPL syntax is similar in formulation to the original mathematical model but also includes syntactic elements from modern programming languages. CMPL is intended to combine the clarity of mathematical models with the flexibility of programming languages.
CMPL executes CBC, GLPK, Gurobi, SCIP or CPLEX directly to solve the generated model instance. Because it is also possible to transform the mathematical problem into MPS or Free-MPS, alternative solvers can be used.
CMPL is an open-source project licensed under GPL. It is written in C++ and is available for most of the relevant operating systems (Windows, OS X and Linux). 

The CMPL distribution contains Coliop which is CMPL’s IDE (Integrated Development Environment). Coliop is an open-source project licensed under GPL. It is written in C++ and is as an integral part of the CMPL distribution. 
The CMPL package also contains pyCMPL, jCMPL and CMPLServer. 

pyCMPL is the CMPL application programming interface (API) for Python and an interactive shell and jCMPL is CMPL's Java API. The main idea of this APIs is to define sets and parameters within the user application, to start and control the solving process and to read the solution(s) into the application if the problem is feasible. All variables, objective functions and constraints are defined in CMPL. These functionalities can be used with a local CMPL installation or a CMPLServer. 

CMPLServer is an XML-RPC-based web service for distributed and grid optimisation that can be used with CMPL, pyCMPL and jCMPL. It is reasonable to solve large models remotely on the CMPLServer that is installed on a high performance system. CMPL provides four XML-based file formats for the communication between a CMPLServer and its clients. (CmplInstance, CmplSolutions, CmplMessages, CmplInfo).  
pyCMPL, jCMPL and CMPLServer are licensed under LGPLv3.
CMPL, Coliop, pyCMPL, jCMPL and CMPLServer are COIN-OR projects initiated by the Technical University of Applied Sciences Wildau.


# Binaries, Examples and Manual

The executable files for Windows, macOs and Linux, examples and the manual can be found under the following link: http://coliop.org


# Installation 

### Linux and Windows 

An installation is not required after unpacking the ZIP or tar.gz file. The CMPL package works out of the box in any folder.   

### macOS

The CMPL package works out of the box in any folder. After unpacking the ZIP file, the following steps must be carried out once at the first start:

 * While holding down the "ctrl" key, click on the icon of the Coliop app and select "Open" from the context menu.
 * In the appearing dialogue, click on "Open".
   
The app is saved as an exception to your security settings so that you can open it in the future by double-clicking on it like a registered app. 
To start CmplShell, the link ```cmplShell``` in  ```CMPLHOME``` have to be double-clicked. In addition a user can do so in Coliop (Menu Actions -> Open CmplShell). 
To start cmpl on the command line please use it within CmplShell.  If ```cmpl.opt``` or ```cmplServer.opt``` need to be edited, just open them via the links in the ```CMPLHOME/opt``` subfolder. 


# Source code

The source code is available on Github:

 * [Cmpl official realeases] (https://github.com/coin-or/Cmpl)  
 * [Cmpl working git] (https://github.com/MikeSteglich/Cmpl2)
 * [Coliop] (https://github.com/MikeSteglich/Coliop)
 * [pyCmpl] (https://github.com/MikeSteglich/pyCmpl3)
 * [jCmpl] (https://github.com/MikeSteglich/jCmpl)


# Support

Please use the GitHub functionalities to post issues and bugs at https://github.com/coin-or/Cmpl/issues or for general discussions at https://github.com/coin-or/Cmpl/discussions.

# Authors 

### CMPL

Thomas Schleiff - Halle(Saale), Germany
Mike Steglich - Technical University of Applied Sciences Wildau, Germany 

### Coliop, pyCMPL and CMPLServer

Mike Steglich - Technical University of Applied Sciences Wildau, Germany 

### jCMPL

Mike Steglich - Technical University of Applied Sciences Wildau, Germany 
Bernhard Knie - formerly Technical University of Applied Sciences Wildau, Germany








