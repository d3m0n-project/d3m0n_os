using System;


namespace d3m0n
{
    public partial class script
    {
        public static object getType(Type type, string value)
        {
            dynamic finalvalue = Convert.ChangeType(value, type);
            object to_return = finalvalue as object;
            return to_return;
        }
    }
}