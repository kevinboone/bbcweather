<h1>bbcweather -- a simple command-line utility to retrieve and format
a BBC weather forecast feed</h1>

<h2>What is this?</h2>

<code>bbcweather</code> is a (very) simple command-line utility 
that dumps a 3-day weather forecast from the BBC Web site with
minimal (or no) formatting. It retrieves the information as 
an RSS feed, parses the XML, and dumps the 
weather information to the console.  
The output
it produces is lightly formatted (bold and underline) to make it
more human-readable, but this formatting is turned off if the
output is redirected, so the 
output can be fed into other scripts
or applications. 
<p/>
This utility was written for use with Linux, but might work on
other Unix variants. It builds cleanly under Cygwin on Windows. 
<code>bbcweather</code> has few dependencies, and builds into
an executable only about 70 kB in size. It therefore loads effectively
instantly, and is a much quicker way to get a brief weather forecast
than starting a Web browser and looking at the BBC Web site
(provided you don't mind using a command prompt). 

<h2>Building</h2>

To build <code>bbcweather</code> you will need the <code>libcurl</code>
development libraries (e.g., <code>yum install libcurl-devel</code>).
Then it should just be a matter of:

<pre class="codeblock">
$ make
$ sudo make install
</pre>

<h2>Usage</h2>

When run without command-line arguments, <code>bbcweather</code> dumps
a 3-day forecast for London, UK. The switch <code>-o</code> selects
current observations rather than a forecast.
<p/>
For other locations, specify a name (which might be full or partial)
as an argument, or a BBC numeric location code. 
So far as I know, the BBC does not publish a list of numeric location
codes. These code can be found by using the BBC Web site to get a forecast
for a specific location, and inspecting the URL used.
<p/>
Names are looked up in a data file, which initially contains a list
of about 100 UK towns and cities. It is expected that users
will modify this list to include locations of interest to
them, and probably remove at least some other entries.
The default location of the data file is
<code>/usr/share/bbcweather/locations.dat</code>.

<h2>Sample output</h2>

<pre class="codeblock">
$ bbcweather -o sheffield 
BBC Weather - Observations for  Sheffield, United Kingdom

Wednesday - 20:00 GMT: Clear Sky, 2°C (36°F)
Temperature: 2°C (36°F), Wind Direction: North Westerly, Wind Speed:
9mph, Humidity: 66%, Pressure: 1018mb, Rising, Visibility: Very Good
</pre>


<h2>Limitations</h2>

Most obviously, the BBC could change the format of its weather feeds, or
the URL of the feed server, at any time. In either of these eventualities,
this utility will need to be modified.
<p/>
The data in the weather feed is not parsed down into individual components
(wind direction, precipitation, etc.) So although it would be easy to make
crude changes to the output format, it would be difficult to make 
fine-grained modifications.

<h2>Legal, etc</h2>

<code>bbcweather</code> is copyright (c)2012-2017 Kevin Boone, distributed
according to the GNU Public Licence, version 3, with contributions
from a number of other authors. Full details are shown in the
source code. The licensing terms mean in essence that 
you may do whatever you like with the software, at your own risk, provided
that the original authorship remains clear. There is no warranty of
any kind. 


