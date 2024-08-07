## Creating a d3m0n application

> ⚠️ A no code application builder is available [here](https://d3m0n-project.github.io/d3m0n_c1/studio.html)

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
  		top_bar="true";
	
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
 		# display text into the host shell
   		# from bash command
		system("echo 'd3m0n path:         $d3m0n'");
		system("echo 'app source path:    $source'");
		system("echo 'app layouts path:   $layouts'");
  
  		# from internal command
  		console.logn("app ressource path: $ressource", "red");
		


	Element.OnClick => exec="python $source/main.py" 
	
this file will be executed in the same time that your layout one


Finnally export your app by calling the script `app_builder.sh`

## Controls
In your d3m0n layout file, you can use multiple controls.

 - [TextBox](TextBox.md)
 - [ListView](ListView.md)
 - [ProgressBar](ProgressBar.md)
 - [CheckBox](CheckBox.md)
 - [RadioButton](RadioButton.md)
 - [RawHtml (⚠️ don't work)](RawHtml.md)
 - [WebView (⚠️ don't work)](WebView.md)
 - [Rect](Rect.md)
 - [Switch](Switch.md)
 - [Text](Text.md)
 - [Image](Image.md)
 - [RoundButton](RoundButton.md)
 - [Button](Button.md)
 - [Hscroll](Vscroll.md)
 - [Vscroll](Hscroll.md)

## Events
You can access your application [Controls](/api.md#controls) with events
> more infos [here](events)

## Embeded-in functions
Use premade functions into your script (`.src`) file
> see [here](functions)

# Apparence
Your system design can be changed to be fancier.

## theme icons

Your d3m0n os apparence can be modified by using a theme.
These ones are located in 

> /usr/share/d3m0n/themes
