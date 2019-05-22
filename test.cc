#include <iostream>

//this file is just to produce lists of numbers, etc, or quickly testing other code

int main(){

	// int num_points = 0;
	//
	// for(double x = -0.45; x <= 0.45; x += 0.1){
	// 	for(double y = -0.45; y <= 0.45; y += 0.1){
	// 		for(double z = -0.45; z <= 0.45; z += 0.1){
	// 			std::cout << x << " " << y << " " << z << std::endl;
	// 			num_points++;
	// 		}
	// 		std::cout << std::endl;
	// 	}
	// }
	//
	// std:: cout << num_points;

	for(double i = -0.5; i < 0.5; i += 0.02)
	{
		std::cout << i << ",";
	}

	return 0;
}
