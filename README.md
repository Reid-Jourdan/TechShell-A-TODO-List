

# TechShell by Group: Agressive TODO-List 


Techshell is a simple linux command-line-interface(CLI) that is able to perform commands, I/O redirection, and error handling. This very *original* project is to mimic the same functions that of a linux terminal. At the higher levels Techshell performs moving throughout directories, creating files, and printing the working directory. At the lower levels Techshell utilizes compartors such as "< > >> ;  | |  &&" with commands. 

Some issues that we faced mainly had to do with I/O redirection. The "execvp" and "execl" function breaks out of the current process. While this wasn't obvious, we thought the parent and child processes should continue and **hypothetically** all of the code below the 'exec' function should have ran. We were making the I/O redirector returning the execute_cmd function back to itself to complete both parts of the command. The solution to this was creating a new fork separately for the commands. So to explain this we would split the lines detect said I/O redirection flag it then if that flag went off we would be able to execute "execvp".
##### Authors: Jay Graham, Reid Jourdan, Nathan Silvernale, Dylan Ronquille
![](https://brand.latech.edu/wp-content/uploads/sites/13/2021/03/favicon-300x300-1.png)

