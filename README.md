SOTE
====

SOTE is a game creation system under development by [Alicrow Entertainment](http://alicrow.org), which aims to provide game developers with a powerful workflow and high-level tools akin to that offered by other game creation systems like Unity or Unreal, but with far greater flexibility.

SOTE is not a complete "game engine" on its own; it's just the top level. It uses the libraries of your choice for rendering, physics, scripting, et cetera. If you have a problem with one of the libraries, you can swap it out for one better suited to you and your project (Note that at this point, you can't really do that. We're gonna get things working using the middleware we want first, and then we'll add the proper abstraction to make things swappable).

Please note that SOTE is still in the very early stages of development, so expect frequent changes, missing features, and general ugliness.

See INSTALL.txt for installation instructions.

After compiling, you can run SOTE either through Code::Blocks, or with the shell script provided for your platform (runSOTE-Linux.sh or runSOTE-Windows.bat) (OS X script will come soon).

It's a good idea to run "git pull" fairly regularly so you have the most up-to-date version of SOTE. And whenever you pull the latest changes, remember to open the project in Code::Blocks and build it again, so you actually update the binary.


Current Library Integrations:
-----------------------------
* Physics: Bullet Physics
* Rendering: OpenSceneGraph
* Scripting: AngelScript
* GUI: CEGUI


License
-------
We haven't decided on a license for SOTE yet, but it will be a liberal open source license of some sort (e.g. MIT, BSD, zlib).


Using the Editor
----------------
The SOTE editor is built into the game. Press F1 to toggle between "Edit" and "Play" modes.

The editor contains a text editor box, which can be used to alter the scene and the objects in it. The data is stored as YAML, with AngelScript for update functions, callbacks, and other scripting needs. Take a look at the sample level (loaded into the editor by default, and located at media/SampleLevel.yaml) to see how things work. It should be self-explanatory for the most part.


The Attachment System
---------------------
A lot of game creation systems (e.g. Unity3d) use a component-entity system. That's a neat idea, but it breaks up the representation of objects, and forces you to refer to simple attribute like position through components instead of just using variables. So SOTE uses an "attachment system" instead, where the common elements (physics body, transform, graphics model, et cetera) are part of the object. There's also inheritance, so you can extend the base GameObject class with additional functionality for particular categories of objects (e.g. A.I. for your NPCs).

If you need something really special, you give the GameObject an Attachment, like Light (casts light) or DangerZone (hurts anyone who touches it).

So an electric fence will be a normal GameObject with a DangerZone attached to it. A lamp is a GameObject with a Light attachment, and if you want to get really crazy, you can have a candelabra with multiple Light attachments for each candle, each with a different offset (that might be a silly idea, but you can do it).

The idea is that Attachments contain functionality that's not specific to any one object type. A lamp might cast light, but so could a guard with a flashlight, or a bonfire, or any number of other things.


Special Note for OS X Users
---------------------------
You may get some broken builds. Not all of us have access to Macs, so we can't easily test every build for you. If it doesn't work, let us know, and we (specifically, Daniel, because he has an old Macbook Pro) can fix it for you.


Special Note for Linux Users
----------------------------
You're awesome.


Special Note for Users of *BSD and Other Operating Systems We Don't Support
---------------------------------------------------------------------------
At the moment, SOTE only has build targets for Linux, OS X, and Windows, but that doesn't mean it won't work for you. Try making a build target for your platform (look at the Linux and OS X targets for help), or just ask us to add support for your platform. Assuming it's a Unix-like system with OpenGL or OpenGL ES, we can probably make it work.

Also, you're awesome.
