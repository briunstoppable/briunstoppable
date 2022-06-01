//console . clouds

var i, value, low_threshold, high_threshold;
var low_count = 0;
var middle_count = 0;
var high_count = 0;
var bPattern = false;	// This will flip flop between our randomization engines below.

for (i = 0; i < 131072; i++)
{
	if (bPattern == false)
	{
		value = Math.random();
		low_threshold = Math.random();
		high_threshold = Math.random();	
	
	if (value > low_threshold)
	{
		if (value < high_threshold)
		{
			process.stdout.write('•');
				middle_count++;
				//console.log('\x1b[37m', 'o' ,'\x1b[0m');
			}
			else
			{
				process.stdout.write('+');
				high_count++;
				//console.log('\x1b[31m', '.' ,'\x1b[0m');
			}
		}
		else
		{
			process.stdout.write('-');
			low_count++;
			//console.log('\x1b[36m', '.' ,'\x1b[0m');
		}
		if (bPattern == false)
		{
			bPattern = true;
		}	
		// console.log(i);
	}
	else if (bPattern == true)
	{
		value = Math.random();
		low_threshold = Math.random();
		high_threshold = Math.random();	
	
	if (value < high_threshold)
	{
		if (value > low_threshold)
		{
			process.stdout.write('|');
				middle_count++;
				//console.log('\x1b[37m', 'o' ,'\x1b[0m');
			}
			else
			{
				process.stdout.write('~');
				high_count++;
				//console.log('\x1b[31m', '.' ,'\x1b[0m');
			}
		}
		else
		{
			process.stdout.write('x');
			low_count++;
			//console.log('\x1b[36m', '.' ,'\x1b[0m');
		}
		if (bPattern == true)
		{
			bPattern = false;
		}	
		// console.log(i);
	}
}
console.log("");
console.log("middle count:	", middle_count);
console.log("high count:	", high_count);
console.log("low count:	", low_count);
