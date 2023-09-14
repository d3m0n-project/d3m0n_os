## Creating a d3m0n application

⚠️ A no code application builder is available [here](https://d3m0n-project.github.io/d3m0n_os/builder)

First create a main file named `app` and type these lines

    name: myapp
    package: com.myteam.myapp
    icon: /path/to/icon.png
    perms: none
    start_path: /path/to
    category: mycategory

### description
`name` is your new app name

`package` is your app 'id'

`icon` is then path to your icon (png) file or a theme icon ([theme-icons](#theme-icons))

`perms` can be equal to `sudo`(admin) or `none`(default)

`start_path` specifiate the directory where your app will be running

`category` is your app category

Then create a main layout file in the directory `layouts` named `main.layout`.

    # d3m0n layout << important!
	
	Window:
		name="My_Window";
		title="My window";
		width="200";
		height="100";
		bg_color="black";
	
	Text:
		content="my text";

create now a main source file in the directory `src` named `main.src`.

    # d3m0n source << important!
	
	# On app load
	Window.OnCreate => 
		# this is a comment
		myVariable="hello world"
		
		# create an alert box with myVariable content
		alert(myVariable);
		
		# write something in the host terminal in orange
		logn("[o] loaded myApp", "orange");
		
		# execute bash command
		system("echo test");
		
		# call a function called "myFunct" with args 'arg1' and 'arg2' in the dll
		[/path/to/myDll.dll,myNamespace,myclass,myFunct(arg1, arg2)]
	

	# on app quit
	Window.OnDestroy => 
		system("echo 'd3m0n path:         $d3m0n'");
		system("echo 'app ressource path: $ressource'");
		system("echo 'app source path:    $source'");
		system("echo 'app layouts path:   $layouts'");


	Element.OnClick => exec="python $source/main.py" 
	
this file will be executed in the same time that your layout one


Finnally export your app by calling the script `app_builder.sh`

## Controls
In your d3m0n layout file, you can use multiple controls.

 - [TextBox](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/TextBox.md)
 - [ListView](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/ListView.md)
 - [ProgressBar](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/ProgressBar.md)
 - [CheckBox](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/CheckBox.md)
 - [RadioButton](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/RadioButton.md)
 - [WebView](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/WebView.md)
 - [Rect](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/Rect.md)
 - [Switch](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/Switch.md)
 - [Text](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/Text.md)
 - [Image](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/Image.md)
 - [Button](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/Button.md)

# Apparence
Your system design can be changed to be fancier.

## theme icons

Your d3m0n os apparence can be modified by using a theme.
These ones are located in 

> /usr/share/d3m0n/themes
