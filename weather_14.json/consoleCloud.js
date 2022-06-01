//console . clouds

// how many iterations?
var iterations = 4;

var i, value, low_threshold, high_threshold;
var low_count = 0;
var middle_count = 0;
var high_count = 0;


for (i = 0; i < iterations; i++)
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
			// color-coded (makes a newline tho...)
			console.log('-', low_threshold);
			console.log('\x1b[37m', value ,'\x1b[0m');
			console.log('+', high_threshold);
			console.log("");
		}
		else
		{
			process.stdout.write('◦');
			high_count++;
			// color-coded (makes a newline tho...)
						console.log('-', low_threshold);
			console.log('\x1b[31m', value ,'\x1b[0m');
						console.log('+', high_threshold);
			console.log("");
		}
	}
	else
	{
		process.stdout.write('·');
		low_count++;
		// color-coded (makes a newline tho...)
		console.log('-', low_threshold);
		console.log('\x1b[36m', value ,'\x1b[0m');
		console.log('+', high_threshold);
		console.log("");
	}
}
console.log("");
console.log("middle count:	", middle_count);
console.log("high count:	", high_count);
console.log("low count:	", low_count);