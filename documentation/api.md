## Creating a d3m0n application

> ⚠️ A no code application builder is available [here](https://d3m0n-project.github.io/studio/)

First create a folder in the app directory that follows [this package convention](./package_convention.md), then add a main file inside named `app` and type these lines

    name: myapp
    icon: default

### description
`name` is your new app name

`icon` is a theme icon ([theme-icons](#theme-icons))

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
		log("[o] loaded myApp", "orange");

	# on app quit
	Window.OnDestroy =>   
  		# from internal command
  		log("app closed", "red");
		


	Element.OnClick =>
		alert("Clicked Element", "red");
	
this file will be executed in the same time that your layout and the events will be registered

## Controls
In your d3m0n layout file, you can use multiple controls.

 - [TextBox](TextBox.md)
 - [ListView](ListView.md)
 - [ProgressBar](ProgressBar.md)
 - [CheckBox](CheckBox.md)
 - [RadioButton](RadioButton.md)
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
