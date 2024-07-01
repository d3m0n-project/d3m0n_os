using System;
using System.IO;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections.Generic;
using System.Drawing;
using System.Threading.Tasks;
using System.Windows;
using System.Reflection;
using System.Linq;

namespace d3m0n
{
    public static class ControlExtensions
	{
	    public static T Clone<T>(this T controlToClone) 
	        where T : Control
	    {
	        PropertyInfo[] controlProperties = typeof(T).GetProperties(BindingFlags.Public | BindingFlags.Instance);

	        T instance = Activator.CreateInstance<T>();

	        foreach (PropertyInfo propInfo in controlProperties)
	        {
	            if (propInfo.CanWrite)
	            {
	                if(propInfo.Name != "WindowTarget")
	                    propInfo.SetValue(instance, propInfo.GetValue(controlToClone, null), null);
	            }
	        }

	        var controls = controlToClone.Controls;
	        for(int i=0; i < controls.Count; i++) {
	        	Control control = processChildren(controls[i]);
	        	var children_controls = controls[i].Controls;
		       	for(int j=0; j < children_controls.Count; j++) {
		       		Control child = processChildren(children_controls[j]);
		       		control.Controls.Add(child);
		       	}
	        	instance.Controls.Add(control);
	        	
	        }
	        return instance;
	    }
	    public static Control processChildren(Control controls)
	    {
			// handle topbar's children elements
			
	    	if(controls.ToString().StartsWith("System.Windows.Forms.Label"))
	        {
	        	Label control = controls as Label;
	        	Label newcontrol = new Label();
	        	newcontrol.ForeColor = control.ForeColor;
	        	newcontrol.BackColor = control.BackColor;
	        	newcontrol.Location = control.Location;
	        	newcontrol.Font = control.Font;
	        	newcontrol.TextAlign = control.TextAlign;
	        	newcontrol.Dock = control.Dock; 
	        	newcontrol.Size = control.Size;
		       	newcontrol.Text = control.Text;
		       	newcontrol.Name = control.Name;

		        return newcontrol;
		       	
	        }
	        if(controls.ToString().StartsWith("System.Windows.Forms.PictureBox"))
	        {
	        	PictureBox control = controls as PictureBox;
	        	PictureBox newcontrol = new PictureBox();
	        	newcontrol.Size = control.Size;
				newcontrol.SizeMode = control.SizeMode;
				newcontrol.Image = control.Image;
		       	newcontrol.Dock = control.Dock;
		       	newcontrol.Location = control.Location;
		       	newcontrol.Name = control.Name;
	       		newcontrol.ForeColor = control.ForeColor;
	       		newcontrol.BackColor = control.BackColor;
		       	return newcontrol;
		       	
	       	}
	       	return new Label();
	   	}
	}
}