

# TechShell by Group: Agressive TODO-List 


Techshell is a simple linux command-line-interface(CLI) that is able to perform commands, I/O redirection, and error handling. This very *original* project is to mimic the same functions that of a linux terminal. At the higher levels Techshell performs moving throughout directories, creating files, and printing the working directory. At the lower levels Techshell utilizes compartors such as "< > >> ;  | |  &&" with commands. 

Some issues that we faced mainly had to do with I/O redirection. The "execvp" and "execl" function breaks out of the current process. While this wasn't obvious, we thought the parent and child processes should continue and **hypothetically** all of the code below the 'exec' function should have ran. We were making the I/O redirector returning the execute_cmd function back to itself to complete both parts of the command. The solution to this was creating a new fork separately for the commands. So to explain this we would split the lines detect said I/O redirection flag it then if that flag went off we would be able to execute "execvp".

Othe issues that we ran into were the file redirection, (<, >, >>). These had issues because when the file is opened for direction into or out of it, the exec function has to be in the same process as it. This was unclear because of how we intially were handling the child processes. IT was extra unclear because we had the file being opened in the child but this would cause problems with the '&&' and other combination commands. we fixed this by sa=etting a flg on which operator was contained in the command and splitting the path to execute the redirection one way and combination the other way.

One thing that is not really handled is having to combination commands in the same line. For example, if the command read "touch file1.txt && touch file2.txt && touch file3.txt" the first file would be created but the commandwill create 'file.txt', '&&', 'touch', and 'file3.txt' files as it see everything after the second touch as arguments to the second command. This could be fixed by checking in the parse command functionto see how many commands should be running and storing them as a list of commands. This would require a large code overhaul so we decided against it for time's sake.

Another thing that is nothandled properly is the '|' operator. This redirects the output of one command into the input of another. It works in the code as is but is done imporperly. Curently it creates a file named 'plzdontnameafilethis.txt' and stores the ouput of the first command and then redirects it into the second command, then deletes the temporary file. There is most likely a way to do this without having a temporary file that is much cleaner and with no drawbacks. The problem with this method is if there was alreday a file named 'plzdontnameafilethis.txt' it would get over written an then deleted.

The history command does not execute properly with this as well due to unknown issues. ALl other commands that we tested work porperly and should function as if it is a normal terminal.
##### Authors: Jay Graham, Reid Jourdan, Nathan Silvernale, Dylan Ronquille
![](https://brand.latech.edu/wp-content/uploads/sites/13/2021/03/favicon-300x300-1.png)

