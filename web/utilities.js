function isEqual(obj1, obj2)
{
  if(obj1 == null || typeof(obj1) != 'object') return obj1==obj2;
  if(obj2 == null) return false;
  if(Object.keys(obj1).length!=Object.keys(obj2).length) return false;

  for(var key in obj1)
  {
    if(obj1.hasOwnProperty(key)) 
      if(!isEqual(obj1[key],obj2[key])) return false;
  }
  
  return true;
}

function clone(obj)
{
  if(obj == null || typeof(obj) != 'object') return obj;

  var temp = new obj.constructor();
  for(var key in obj)
    if (obj.hasOwnProperty(key)) temp[key] = clone(obj[key]);

  return temp;
}
