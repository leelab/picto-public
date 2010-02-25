#include "State.h"

namespace Picto {

State::State()
{
	type_ = "State";
}

QString State::run()
{
	//This is where the rendering actually happens
	while(true)
	{
		//Draw the scene
		//! \TODO implement Scene::draw()

		//check all of the control elements
		//This foreach loop was causing the compiler to crash.
		//Probably due to the fact that it was empty
		//foreach(ControlElement control, controlElements_)
		//{
			//! \TODO implement the control elements here
			
			/* PSEUDOCODE
			if(control.isDone())
				return control.result();
			*/
		//}

		//If there are no control elements, then we just return with an empty string
		//if(controlElements_.isEmpty())
			return "";
	}
}
}; //namespace Picto
