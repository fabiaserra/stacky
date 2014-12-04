stacky
======
Simple game using ofxKinectCommongBridge and ofxBox2D.

Written for OF 0081.

There are some details that should be polished but haven't had the time to do: 
  - Amount of boxes/black circles that fall in Easy Mode vs Hard Mode 
  - Sound interactions
    - Add sound when impact between black circles and plates
    - Add sound when impact between box and plate when some boxes are already attached in the plate
    - Multiple instances of the impact sound when contact between box and the boxes attached in the plate

Some of these problems are derived because of the method used to attach the boxes to the plates. When using the physical properties of the objects with ofxBox2D, the boxes were not following the pile on top of the plate (boxes not reacting to friction with the plate), that's because we are assigning the plate position to the skeleton wrist joint directly, loosing all the physical properties of the plate. I tried different solutions but finally what I do is attach the falling boxes to a pile that moves together if its center falls within a specified distance range of the box below it. If we move the pile very fast the boxes are disattached from the pile and hence they fall from it.

Example video: http://youtu.be/X6Zw_NDU5Ag


