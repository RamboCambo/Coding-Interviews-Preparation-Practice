Executables were built on WSL 2 Ubuntu 22.04 using gcc with the standard set to c99. See makefiles for details. No special binaries were used.

Client:
Navigate to the src/client folder in terminal
Run "make"
Navigate to ../../bin
Run the client using the following format: game_client <Game Type> <Server Name> <Port Number>

Server:
Navigate to the src/server folder in terminal
Run "make"
Navigate to ../../bin
Run the client using the following format: game_server <Port Number> <Game Type> <Game arguments>
