# Ams2o
## A string-crunching tool based on Amscript2


### How To Build

1. Navigate to the project's root directory (the one containing <code>makefile</code>)
2. Run the following Bash commands:
   1. <code>make Amscript2/lib/libamscript2.so</code>
   2. <code>make -j8 bin/ams</code> (the <code>-j</code> flag is optional, but it
      allows <code>make</code> to compile multiple files at once)


### How To Install *(Linux only)*

#### With administrative privileges

1. Run the following Bash command: <br/>
   <code>make install</code>
2. Type your password for <code>sudo</code> to use, if prompted

Alternatively, for a manual installation:

1. Move <code>bin/ams</code> to <code>/usr/local/bin/ams</code>, or to
   <code>/usr/bin/ams</code> if the <code>/usr/local/bin</code> directory
   is not in the Bash search path
2. Move <code>Amscript2/lib/libamscript2.so</code> to
   <code>/usr/local/lib/libamscript2.so</code>, or in any directory where
   the system looks for shared libraries
3. Run <code>ldconfig</code>, or reboot the system

#### As a regular user

A regular user cannot perform a system-wide installation, therefore it's up
to the user to follow the steps above (for the manual installation) while
using directories where the system looks for user-specific executables and
libraries.

For example, on **Ubuntu** the user should:
1. move <code>bin/ams</code> to <code><i>$HOME</i>/bin/ams</code>;
2. create a library directory in their home and add it to the
   <code><i>LD_LIBRARY_PATH</i></code> environmental variable, unless it has
   been done before;
3. move <code>Amscript2/lib/libamscript2.so</code> to the aforementioned
   directory


### How To Use

1. Write a Amscript2 file
2. Define the Ams2o options, if necessary (use the table below)
3. Run <code>ams</code> using the file as input
   - use the file path as a command line argument. For example: <br/>
     <code>ams path/to/file</code>
   - run <code>ams</code> without arguments, and pass the script file
     to the shell's standard input. For example: <br/>
     <code>ams &lt; path/to/file</code>
     OR
     <code>echo 'hw: "Hello, world!" hw' | ams</code>


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
</p> <p>
	Note: options must be defined as functions in order for the program
	to read them correctly.
</p>


# Ams2o Script Example

<pre>// Example Ams2o script

ams2o::mode () :      { ams2o::text }
ams2o::separator     -> ' + '
ams2o::show_warnings -> ams2o::false

hello(whom): { 'Hello,' whom '!' }

hello('world') "\n"
'The ASCII code for \'a\' is' ams2o::ascii('a') "\n"</pre>
