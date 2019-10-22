# Ams2o
## A string-crunching tool based on Amscript2


### How To Use

<p>
1. Write a Amscript2 file
2. Define the Ams2o options, if necessary (use the table below)
3. Run <code>ams2o</code> using the file as input
   - use the file path as an command line argument. For example: <br/>
     <code>./ams2o path/to/file</code>
   - run <code>ams2o</code> without arguments, and pass the script file
     to the shell's standard input. For example: <br/>
     <code>./ams2o &lt; path/to/file</code>
     OR
     <code>echo 'hw: "Hello, world!" hw' | ./ams2o</code>
</p>


### Utility functions

<table>
	<tr> <th>Name (Arguments)</th> <th>Description</th> </tr>
	<tr>
		<td> ams2o::ascii </td>
		<td>
			Converts ASCII strings into sequences of numbers, and
			numbers to ASCII characters.
		</td>
	</tr>
	<tr>
		<td> ams2o::concat </td>
		<td>
			Concatenate multiple values without adding a separator
			between each value.
		</td>
	</tr>
</table>


### Ams2o Options

<table>
	<tr> <th>Name</th> <th>Values</th> <th>Default value</th> </tr>
	<tr>
		<td> ams2o::mode </td>
		<td>
			ams2o::text <br/> ams2o::binary
		</td>
		<td> ams2o::text </td>
	</tr>
	<tr>
		<td> ams2o::byte_order </td>
		<td>
			ams2o::small_endian <br/> ams2o::big_endian
		</td>
		<td> ams2o::big_endian </td>
	</tr>
	<tr>
		<td> ams2o::word_size </td>
		<td>
			ams2o::int8 <br/> ams2o::int16 <br/>
			ams2o::int32 <br/> ams2o::int64
		</td>
		<td> ams2o::int8 </td>
	</tr>
	<tr>
		<td> ams2o::show_warnings </td>
		<td>
			ams2o::true <br/> ams2o::false
		</td>
		<td> ams2o::true </td>
	</tr>
	<tr>
		<td> ams2o::separator </td>
		<td>
			<i>any value</i>
		</td>
		<td> <i>empty string</i> </td>
	</tr>
</table>

<p>
	To set an option's value, include it in the script as a definition. <br/>
	For example, to disable warnings, write the following line:
	<code>ams2o::show_warnings: ams2o::false</code>
</p>


# Ams2o Script Example

<pre>
// Example Ams2o script

ams2o::mode: ams2o::text
ams2o::separator: ' + '
ams2o::show_warnings: ams2o::false

hello(whom): { 'Hello,' whom '!' }

hello('world')
'The ASCII code for \'a\' is' ams2o::ascii('a')
</pre>
