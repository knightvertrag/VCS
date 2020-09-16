# <center/>Imperium- VCS

## <center/>Introduction



>**In software engineering, version control (also known as revision control, source control, or source code management) is a class of systems responsible for managing changes to computer programs, documents, large web sites, or other collections of information. Version control is a component of software configuration management.**

~ The ever reliant Wikipedia.

In this 10 day programming exercise, this guide isn't meant to be a tutorial for you. Think of it as a document which will provide you a gentle nudge in the right direction. 

This guide will help you visualise the procedural complexity of a VCS and we will try to emulate the functionality of the OG VCS of them all - 

**<center>GIT.</center>**

<br/>

We will go through the following steps in the course of this tutorial:

<li/> Shell Scripting With Bash
<li/> Initialising an empty Imperium Repo (Init)
<li/> Staging Changes for the Commit (Add)
<li/> Commiting them (Commit)
<li/> Checking out to a specific commit (Checkout)
<li/> Conclusion and further innovations possible.

___

## <center/>Making your own CLI

The first step towards making a functional version control system is to prepare your own command-line interface (imperium add, imperium commit, etc).

So to proceed we need two shell scripts, one to build your project and another to run your commands in the terminal. The final goal is that the user will download your source project, and run the build script to “install” the VCS. Once this is done, the imperium terminal commands become available to be used. How exactly this can be achieved will be discussed shortly.

The second shell script is what will be executed every time you type an imperium command in your terminal. This script basically acts as a middleware between your commands and the C++ executable that you built with the build script.

In this project, we will have a main executable file which needs to be run each time your self-made commands are used. We do not want the user to manually compile and run the C++ program themselves every time they want to perform an operation. This needs to be automated through a shell script.

We also want our commands to run from any directory, not just the root directory of our project, and hence it needs to be installed in our system.

<br/>

## <center/>Steps in the build script

1. First you need to install two C++ libraries required for SHA-1 hashing, openssl and libssl-dev.
2. Create a directory in the `$HOME` folder to store the compiled C++ program binary and our command script.
3. Copy the command script from VCS source code to the created directory.
4. Go the VCS root where the Makefile is and make the project.
5. Now cd to the new `$HOME` directory and change permission of the binary to executable.
6. Put the command script in the `~/.bashrc` to permanently add it to your system `PATH`.

## <center/>Steps in the command script

1. Create a function called imperium(or whatever you wanna name it). We are using a function here because we can `source` it in the `.bashrc` and that allows us to call `imperium` without writing the `.sh` extension, thus giving a real command line feel.The `.bashrc` entry will look something like this(call this in the build script).

```bash
echo "source $PWD/imperium.sh" >> ~/.bashrc;
```

1. The imperium function will looks something like this:

   ```bash
   function imperium(){
    DIR=$PWD
    export dir=$DIR
    cd ~/imperium/bin || echo "Error"
    ./main "$@"
    cd "$DIR" || echo "Error"
   }
   ```

   As you can see, we first store and export the path of the current working directory as an environment variable. This is required as all our paths being used have this directory as the root.

   After that we cd into the `$HOME/imperium/bin` folder where the compiled C++ binary is.
   The next step is to execute the binary and pass in all the commands as command line parameters so we can figure out what command is being called by the user.

   Once this is done simply cd back to your repository directory.

<br/>

# <center/>init() function

The first command the user enters is `imperium revert init` to initialize the folder as an imperium repositiory. The function to handle this process is fairly simple. Lets see what we need to do step-by-step:

1. First check if the folder is already initialized as a repo. To do this we simply check if the `.imperium` folder exists already or not. If it does we just print a message and return to main() function.
2. If the previous check returns `false` then we move on to create a new file and folder, namely:

   1. `.imperium` folder
   2. `.imperiumIgnore` file (**optional**)

   The `.imperium` has all the data and logs required for our VCS to function and the `.imperiumIgnore` file acts like a `.gitignore` file, it tells our program which files to not track.

3. Next we populate the `.imperiumIgnore` file with some common files and folders:

   1. `/.imperium`
   2. `/.imperiumIgnore`(**optional, but highly recommended**)
   3. `/node_modules`
   4. `/.env`

   Keep in mind that `/.imperium` **has** to added to the file for it to work correctly.

4. Now we move on to the `.imperium` folder and start populating it.
5. Create two files called `.commit.log` and `add.log`. These will be used to keep track of the commits and the add cache.

Finally print a success statement.

# <center>IMPERIUM ADD</center>

### Going into this project, we wanted to demonstrate how intuitive a system can be made with some logic and gritty old hardwork. imperium might not have the efficiency of Git, but it surely will do the job.

<br/>

For add, we will be dividing this part into 4 sub-parts:

1. The add function itself

2. A function to check if the file already exists on the log

3. A function to check if the path is supposed to be ignored (optional, see this only if you chose to initialize an ignore file, and used it.)

4. a function to copy the file on the specific path to another directory.

## ***A quick note about the special files, and folders.***
1. You might remember we initialised a add.log file in the init function, in the .imperium directory in your project root. For the purposes of easy reading and understanding, whenever we refer to the add log, we are indeed referring to that file.

2. We will be initialising a folder named .add in your imperium folder at a certain step, so bear in mind the difference between the two.

### So let us begin.
<br/>

# <center>1. The add function</center>

Ideally, as it so happens in git, you type,

```bash
git add [name of folder or path to file]
```
## We will be following that route as well. 

Remember how we asked you to pass the location of the command call from the bash to your main program? yes, you should store that somewhere, in your program as a global variable, we'll be using it a lot.

For all intents and purposes, i'll be referring to that supposed variable as `root`. 

In git, if you type -

```bash
git add .
```
### <center>OR</center> 
<br/>

```bash
git add [path]
```

So first, just check if the argument passed after add is a '.' or not.

If it is a period ( ```.``` ), then the path for which you check is the `root`.

If not, then the path for which you check is `root`+(the argument passed).

And yes, do store this path in a new variable. i'll call it `addPath` here.

First things first, you should check if the new `addPath` exists on the disk or not.

If yes, check if the `addPath` is to be ignored or not. (Function 2).

### I'll discuss that logic later.

___

<br/>

Check if this path is for a file or a folder.

### If it's a file, go directly to SUB-PART B.

<br/>

## <center>**Subpart A**</center>

Now, the chances are pretty high that whatever language you're coding in, has a function for going through a directory recursively. Going through a directory recursively means going through all the paths, and sub-paths and files and listing all the absolute paths one at a time.

<br/>

C++ has various functions in the
```c++
<filesystem>
```

header. (available C++ 17 and up, make sure to be on the latest gcc)

Check [this](https://www.bogotobogo.com/python/python_traversing_directory_tree_recursively_os_walk.php) article for Python.

So essentially you'll be generating links for all the sub-folders and files inside this folder and individually checking them. The procedure for that is in Subpart B below.

<br/>

## <center>**Subpart B**</center>

So let's just assume that such a function (discussed in function 2) already exists and work perfectly, and they tell you if the path is valid or not. if it is supposed to be ignored, that means one of the next two things:

a. The path is already in the addLog.

b. It is specifically mandated to be so in your ignore file.

If that file is Good To Go, add the file to your add log, and copy the file/folder on the path to your add folder(function 4)

<br/>

# <center>2. Function toBeIgnored</center>

This function just checks if your current path, is supposed to be ignored for addition.

The idea goes like this..

We will pass a path in this function.

We will call this variable `paramPath`
~~~c++
toBeIgnored(paramPath){
 //business logic here
}
~~~

<li/> Read the add log.

<br/>If `paramPath` exists in addLog then:

1. Copy the files again over writing the files already present in add. (function 4) 

    ***[why this is important is that you made changes to an already added folder/file, and wish to stage those changes as well]***

2. Return True (i.e, the File is to be ignored to avoid re addition to the addLog. Correspond with *Subpart B in Fn 1.*

If `paramPath` does not exist in add log, call `ignoreCheck` function (**Skip this if you haven't decided to make a custom ignore file, also this corresponds to Fn 3**)

1. If this function returns true, return true, (i.e, this path should be ignored) else return false.

# <center>3. Function ignoreCheck [optional]</center>

#### *If you'd decided to define your own rule of a ignoreFile, like gitIgnore, then just check if the path passed to this function exists in the ignoreFile. For further improvement, you can mandate the function to return false for any specific file type, foldername or filename, throughout the system.*

<br/>

We will pass a path in this function.

We will call this variable `checkPath`
~~~c++
ignoreCheck(checkPath){
 //business logic here
}
~~~

Read your ignoreFile.

## **Return true if the file/folder on the path fails your requirements. Else return false**

<br/>

# <center>4. The addToCache function</center>

### ***The way this works is simple. You pass a path to this function, and you extract the relative path w.r.t to the project folder.***
<br/>

#### *Whenever you call this function, make sure the .add directory exists in the .imperium folder, if it doesn't, create it.*

<br/>

We will pass a path in this function.

We will call this variable `addPath`
~~~c++
addToCache(addPath){
 //business logic here
}
~~~

### **Suppose your root folder (project root) is at :** *home/username/projects/project1*

### **And your file to be copied is :** *home/username/projects/project1/sub-folder1/a.css*

### **Then the relative path is :** */sub-folder1/a.css*

<br/>

***You have to copy the file from it's inital location to the .add folder in your .imperium folder.***
i.e, copy the file from:

***home/username/projects/project1/sub-folder1/a.css*** 
<center>to </center>

***home/username/projects/project1/.imperium/.add/sub-folder1/a.css***

As you can see the relative path stays the same. You have to add checks to make sure, that the sub-folder1 folder, and any other nested directories, already exist before you attempt to copy a.css, or you'll terminate your program with a runtime error, or worse, mess up the structure of where your files are supposed to be, and copy them to a wrong location.

So, that was add. What we will work on next is commit, where in we simply copy the files from the .add folder to a newly generated folder with a unique name in our commits directory.

<br/>

# <center/>IMPERIUM COMMIT 

*At this point you are expected to have built a .add folder which contains all the files staged for commit and an add.log file containing a log of these staged file/folders. You can keep adding to these until you feel like your code is ready to be archived.*

### When you commit your changes, it means that a copy of the current state of your project has been saved in your system and can be revisted at any stage in the future if the need/wish to do so arises. 

There are several ways in which you can go about with this step. Our easy, albeit memory inefficinet, approach is to store the entire project in a directory inside a **.commit** folder.
<br/>

> The names of these directories have to be *unique* as they will be what will help us identify and retrieve your previous commits. 

#### The commit command takes in a commit message like this- `imperium commit -m “message”` - this can be customised and for the sake of simplicity we will exclude the ‘-m’ flag. 

Having understood these key points, we are now ready to write a code to commit our changes!

### In the main function

**_In the main function check if the user is trying to commit the changes and if yes, call the commit function and pass the command arguments as paramenters._**

## <center/>Function commit()

#### A couple of checks before we get started-

- As a safety measure, check if the repo is initialised as an imperium repo, return an error if not.
- Extract the commit message passed in the argument and check if it is null, throw an error if it is, as we need messages in order to ease the differentiation between different commits.

If both the checks pass, the next step is to **generate a commit hash** which is unique for each commit.

> Now there can be various information such as author name, changes made, timestamps,etc that can make up the hash. Since we are not building a distributed system, we need not store the author name.

As we only want to ensure the uniqueness, we can use the time of commit creation as a factor. Make sure you are measuering it at least till seconds- the more presise, the better.

#### Different languages have different ways to take in the system time-

1) **For cpp** : You can use the **_chrono_** and **_ctime_** libraries.
2) **For python** : **_datetime_** library will do the job.

Once you have the date you need to turn it to a hash. We have used SHA1 hashing algorithm, u can use any other if you want to.

1) **For cpp** : You can use the **_openssl_** library supported by cpp. **You will need to install openssl and libssl-dev** and add the flags `-lssl -lcrypto` while compiling. Add these to your build scripts. Read [this](https://stackoverflow.com/questions/918676/generate-sha-hash-in-c-using-openssl-library) for more insight.

2) **For python** : **_hashlib_** will serve the purpose Read [this](https://www.programiz.com/python-programming/examples/hash-file) for more info.

## Now that we have the hash, we can now create a folder with this name and store the commited files.

- ### Recall that we have a .add folder which is being used to stage our chnages. This is the folder we need to look at right now. *Changes not staged for commit will not be committed.*

- ### Since we are storing the entire project in each commit folder, we will also need to get the unmodified files which had earlier been committed.

- ### We also need to keep a track of all the commit hashes and their messages in a commit log.

#### Too many points to keep in mind right? Lets do it one small step at a time.

**_Lets assume that this is the first commit. So now we have some files and folders in the .add folder and we need to permanently store it in a .commit folder. This is essentially a copy paste mechanism from .add to .commit and is easily achievable using the recursive directory iterator you have used before._**

### Starting from the root + /.imperium/.add folder, recursively go through each path inside this .add directory.

#### At each iteration, the recursive directory iterator will return the absolute path of the file/ folder (lets call it **absPath**) it is currently going through. Check whether it is a file or a folder (as you did while writing your add function) and lets say we are storing it as 'f' for file or 'd' for directory in a variable called _type_. Lets make a new function which deals with the copy pasting and call it addCommit(). Call this function and pass absPath, the type(that is file or folder) and the commit hash.

## <center/>addCommit() function

Create a .commit folder if it does not exist. Inside this .commit folder, create a new folder with the name as the passed commitHash. 

#### All the paths that will be passed to us will be of this form – `root + /.imperium /.add /filename`. We need to transfer this to `root + /.imperium /.commit /commitHash /filename`. 

##### So for each path, extract the filename from the absolute path and create its new path (the one in the .commit folder) in another variable, say filerel.

```
filename= part of absPath after root+/.imperium/.add/
filerel= root+ /.imperium/.commit+commitHash
```

>If **_type ='f'_**, copy the file from the absPath into this filerel. If **_type='d'_**, create the filerel directory. 

By the end of this recursion, you will have a copy of your staged changes in your .commit/commitHash folder.


## Coming back to the commit function
### _Now the .add and add.log have served their purpose and need to be deleted. So after the recursive call, delete both of them._

### Just one more thing left to do!

#### Adding this commit's details to the commitLog!

Create a function, lets call it updateCommitLog() and pass the commitHash and the commit message as arguments. 

## <center/>updateCommitLog()

**Remember how we created a .commiLog file inside .imperium at the time of init? Now is the time to populate it. You need to write into the commit.log file, the following** (_the steps to write into a file are not being discussed as it has already been coverd before_)**- the commit hash and the message. Also, the lastest commit should have a flag called HEAD.**

#### So you commitLog will look comething like this-

```
commit hash3 -- commit message3 --> HEAD
commit hash2 -- commit message2
commit hash1 -- commit message1
```
>This will help with tracking latest changes in the furture. 

##### NOTE: Your work will become easier if you add the new commits to the top of the commitLog file instead of appending to it
##### For this, the easiest way is to create a temporary file,say temp.log. Write the latest commit hash, message and the HEAD flag to this file. Then read the .commitLog file line-by-line and copy it into temp.log. Make sure you do not add the HEAD flag which will be present in the first line of commitLog. Once the file is copied into temp.log, delete .commitLog and rename temp.log to .commitLog. 

#### _With this done, you have a completely functional code for first commit!_

### But wait thats not all!

**Remember that this was only for the first commit.** _Lets say we have progressed a lot with our project and have made several commits. This time the_ **.add** _folder will only have those files/folders whch have been modified/newly created._

#### So following our older algo, we will be archiving only these changes. However, we are not aiming for this. We are instead looking for a way to store the entire status of the project.

##### Hence, only recursively going through .add is not enough. We also have to now checkout the last commit to copy the file/folders which were previously commited but not staged this time.

#### In your commit function check if this is the first commit by checking for the existance of a .commit folder or commit.log file. If they exist, retrieve the HEAD commit’s hash, which, if you added the latest hash on top instead of appending, will be the first line of .commitLog (your work has become easier!). Lets store this as headHash.

#### Now recursively iterate through the root+ /.imperium /.commit /headHash folder and call the addCommit folder.

## <center/>addCommit() again!
This will almost remain the same. The only changes are that **instead of copying from the .add folder, we are now copying from the .commit/headHash folder.** 
##### You can check for this in different ways- 

* pass another parameter which tell you whether you are in iterating through .commit or .add
* check if absPath has .add or .commit
* create a different function for this case

**Suit yourself!**

Another check that we need to put in this case is the **deletion of folders between the last commit and current commit**.
#### Our .add folder is not keeping a track of such deletions and hence in this case of copying from the previous commit, before we write to the new commit, we need to check if the folder/file still exists in the root directory.

##### For this, just retrieve the filepath after `root+/.imperium/.commit/commitHash`. Check if root+filepath exists. If it does, add it to the .commit/commitHash, else it has been deleted since the last commit and should not appear in the new commit. 

## <center/>Back to commit()

**_After this recursion gets over, you have the code for going over the .add folder like you did for the first commit and overwrite the changes in the commit folder as per the staged files._**

## Now you have a fully working commit function keeping a track of various stages of your commit and... we are done!!
<br/>

# <center/>Checkout

One of the primary reasons for using a Version Control System is the assurance that you can always go back to a past commit if you fuck up your code. And thus, our VCS won't be worth its name unless it supports checking out to any previous commit you want.

## <center/>The Prerequisites

So we already have the commit system set up. Lets review the components we created for it.

1. A commit log that tracks the hash of all commits created.
1. A head log containing the hash of the latest commit, or, the head commit.
1. Folders containing the snapshot of the project when the commit was made. We map the folders to their respective commit by having the folder name as the commit hash.

Keeping the above mentioned points in mind, we now need to create a checkout system that will yeet the project back in time to the point your life was nice.

## <center/>Time Travel

The idea is simple, we have the project's code and structure that we need to go back to, so all we have to do is replace the current project root's contents with the contents of our required commit folder. Let's take a step by step approach to achieving this.

1. ### The user needs to enter the checkout command and the hash of the commit he/she wants to checkout to

   So this step is pretty obvious, we need the hash to pick the right commit folder. So we create a command:

   ```bash
   imperium checkout hash
   ```

   As with all other commands, we check for the second argument to figure out what the user wants to use. Here, `checkout` tells the program that we want to checkout to a commit and `hash` tells it which commit to checkout to. However, SHA-1 hashes are 40 characters long and completely random. There's no way the user is going to remember every commit's hash. So we need a way to list the commit's name and its hash so that the user can copy the required hash and paste it after `checkout`.

   So let us start by first creating a function for listing all commits. This is reallt simple as all we need to do is list the contents of the `commit.log` file.

   So our fuction will look something like this:

   ```C++
   void getCommitLog()
   {
     file = "/.imperium/.commit/commit.log"
     file.read();
     while(!file.eof())
     {
       print(file.line);
     }
   }
   ```

   We can call this fuction with a command like `imperium commit-log`.

   Upon entering this commmand we get list printed on the terminal:

   ```bash
   a3a6d8371111382a09c4832fcd5ff15abd8ee3c5 -- second comit -->HEAD
   bcccc3d84231bb2188723fa3eed2a7f574abcd69 -- first comit
   ```

   So a `checkout` command will look something like this:

   ```bash
   imperium checkout bcccc3d84231bb2188723fa3eed2a7f574abcd69
   ```

   Now that we have the hash we need to work on the `checkout` function itself.

 ## <center/> The checkout() fuction

   The `checkout()` function allows us to go back to a particular commit's state of the project. The idea is to go through the commit folder and copy-paste its contents in the project root. This does not touch any folders or files that have been created after our required commit, it just overwrites files existing in both the current root and the copmmit folder and creates files that no longer exist in the root but are there in the commit folder. So keeping this in mind, lets see how your function might look like.

   ~~~C++

    void checkout(char **argv)
    {
        commitLog = root + "/.imperium/.commit/commit.log";
        commitLog.read();
        //Go through the log and checl if hash exists
        directoryExists = false;
        while(!commitLog.eof())
        {
            if(commitLog.line() == argv[2])
                directoryExists = true;
        }
    }
~~~

   Once we have validated the hash, we need to recursively copy the commit directory to the root, overwriting files wherever needed. So continuing the function:

   ```C++

    void checkout(char **argv)
    {
        commitLog = root + "/.imperium/.commit/commit.log";
        commitLog.read();
        //Go through the log and checl if hash exists
        directoryExists = false;
        while(!commitLog.eof())
        {
            if(commitLog.line() == argv[2])
                directoryExists = true;
        }
        commitFolderPath = root + "/.imperium/.commit/" + argv[2];

        //Recursively Copy and overwrite existing
        copy(from : commitFolderPath, to : root, overwrite : on);
    }

   ```
# <center/>CONCLUSION

This system despite working as promised, is no where near the sophistication of Git. As a starting Dev, you might or might not have noticed a few bugs in our approaches. Nevertheless, here's a list of a few of them below:

1. The space complexity. Git uses patches. As in, it stores only the changes, in the specific files across commits, and rebuilds your files at a specific point by sequentially going through them.
2. The current implementation of add, doesn't track file deletions at all.
3. Compression. Try copy-pasting a 1 GB file over and over. You wouldn't like it. Not one bit.


## <center/>Innovations you can bring about:

1. Switch to a patch system.
2. Add the concept of branching
3. Use a data structure like a linked list instead of our logs.
4. Add functions for handling distributed commits, and merge conflicts.
4. And if you're still here, then add networking. That's the ultimate step. Various computers over a network, using your VCS to share code. That's the dream.

*This project was intended to set the ball rolling. With a bit of programming and file knowledge in any language, you've already built a system which works moderately well, and covers most of the basic testcases of Git's usage. Yes, it isn't a Git replacement. It has taken years, and several thousand commits to get there. So will you, if you keep an open mind.*

We have ourselves worked on enabling a few of the innovations we've talked about here. The quality of code you spin up after following this guide, is already present on our repo on GitHub, and we're constantly working on improving it. And as always, you're always welcome to contribute there. 

Thank you for your participation.

This has been a GLUG presentation.
