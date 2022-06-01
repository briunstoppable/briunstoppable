// Define JSON File
 var fs = require("fs");
 console.log("\n *STARTING* \n");
// Get content from file
 var contents = fs.readFileSync("new7.json");
// Define to JSON type
 var jsonContent = JSON.parse(contents);
// Get Value from JSON
 console.log("city:", jsonContent.city.name);
 console.log("temp:", jsonContent.main.temp);
 console.log("min:", jsonContent.main.temp_min);
 console.log("max:", jsonContent.main.temp_max);

 if (jsonContent.main.temp > jsonContent.main.temp_min)
 {
	 console.log("temp is higher than minimum.");
	 if (jsonContent.main.temp < jsonContent.main.temp_max)
	 {
		 console.log("temp lower than maximum.");
		 console.log("this is our desired/expected zone");
		 console.log('\x1b[37m', '.' ,'\x1b[0m');
	 }
	 else
	 {
		 console.log("temp is higher than maximum. Go Red!");
		 console.log('\x1b[31m', '.' ,'\x1b[0m');
	 }
		 
 } 
 else
 {
	console.log("temp is lower than minimum, go blue.");
	console.log('\x1b[36m', '.' ,'\x1b[0m');
 }