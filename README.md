# RPi-VideoPlayer
GPU accelerated ffmpeg based GTK3 video player

Source code is based on examples taken from https://jan.newmarch.name/LinuxSound/Diversions/Gtk/
Needs ffmpeg installed.
GPU supported hardware acceleration was added for YUV->RGB conversion on the Raspberry Pi 3.
Sound stream is played with ALSA. 10 band cascaded biquad equalizer is implemented based on example at http://www.musicdsp.org/showone.php?id=64.
Multiple cores of CPU are used for threads.
Playlist is stored in SQLite3 database.

Please feel free to use the source. Any comments are welcome.
