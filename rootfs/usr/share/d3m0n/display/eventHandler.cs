using System;

namespace display
{
    // Define a delegate for the event
    public delegate void CustomEventHandler(string message);

    // Class that contains the event
    public class EventPublisher
    {
        // Define the event of type CustomEventHandler
        public event CustomEventHandler CustomEvent;

        // Method to invoke the event
        public void InvokeCustomEvent(string message)
        {
            // Check if there are any subscribers to the event
            if (CustomEvent != null)
            {
                // Invoke the event by calling all subscribed methods
                CustomEvent(message);
            }
        }
    }

    // Example usage
    public class Program
    {
        // public static void Main()
        // {
        //     // Create an instance of the EventPublisher
        //     EventPublisher publisher = new EventPublisher();

        //     // Subscribe to the CustomEvent
        //     publisher.CustomEvent += HandleCustomEvent;

        //     // Invoke the CustomEvent
        //     publisher.InvokeCustomEvent("Custom event invoked!");
        // }

        // Method that will handle the CustomEvent
        public static void HandleCustomEvent(string message)
        {
            Console.WriteLine("Custom event handled: " + message);
        }
    }
}