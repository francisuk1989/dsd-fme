
# Digital Speech Decoder - Florida Man Edition

DSD-FME is an evolution of the original DSD project from 'DSD Author' using the base code of [szechyjs](https://github.com/szechyjs/dsd "szechyjs"), some code and ideas from [LouisErigHerve](https://github.com/LouisErigHerve/dsd "LouisErigHerve"), [Boatbod OP25](https://github.com/boatbod/op25 "Boatbod OP25") and 
[Osmocom OP25](https://gitea.osmocom.org/op25/op25 "Osmocom OP25") along with other snippets of code, information, and inspirations from other projects including [DSDcc](https://github.com/f4exb/dsdcc "DSDcc"), [SDTRunk](https://github.com/DSheirer/sdrtrunk "SDRTrunk"), [MMDVMHost](https://github.com/g4klx/MMDVMHost "MMDVMHost"), [LFSR](https://github.com/mattames/LFSR "LFSR"), and [EZPWD-Reed-Solomon](https://github.com/pjkundert/ezpwd-reed-solomon "EZPWD"), Eric Cottrell, SP5WWP and others. Finally, this is all brought together with original code to extend the fuctionality and add new features including NCurses Terminal and Menu system, Pulse Audio, TCP Direct Link Audio, RIGCTL, Trunking Features, LRRP/GPS Mapping, P25 Phase 2, etc, OP25 Capture Bin compatability, etc. DSD-FME is primarily focused with Linux Desktop users, so please understand that this version may not compile or run correctly in other environments.

This project wouldn't be possible without a few good people providing me plenty of sample audio files to run over and over again. Special thanks to jurek1111, KrisMar, noamlivne, racingfan360, iScottyBotty, LimaZulu and others for the many hours of wav samples submitted by them. Most importantly, HRH17, whose insight, information, samples, and willingness to let me remote into a computer half-way across the globe in order to test trunking features are what make DSD-FME what it has become. Thank you everybody.

Click image below for video of current DMR/P25 trunking capabilities.

[![DSD-FME](https://github.com/lwvmobile/dsd-fme/blob/dev/dsd-fme.png)](https://www.youtube.com/watch?v=hGIijBbbUxg "DSD-FME Trunking Demo")

![DSD-FME](https://github.com/lwvmobile/dsd-fme/blob/dev/dsd-fme2.png)

![DSD-FME](https://github.com/lwvmobile/dsd-fme/blob/dev/dsd-fme3.png)

## Information

See the example folder for information on cloning, installing, example usage, and trunking features.

## Notice

The pulseaudio branch will hearby be for legacy use as the promotion of this version to main branch will obsolete that branch. Any pre-compiled windows versions found in the release link will be considered legacy as well, and do not feature trunking support and have older handling for some other modes. Users are free to use those, but keep in mind the use case syntax may and will be different between versions. Furthermore, any wiki articles made for windows users may contain outdated information for building the pulseaudio branch, please adjust any information found in those accordingly.

## License
Copyright (C) 2010 DSD Author
GPG Key ID: 0x3F1D7FD0 (74EF 430D F7F2 0A48 FCE6  F630 FAA2 635D 3F1D 7FD0)

    Permission to use, copy, modify, and/or distribute this software for any
    purpose with or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
    REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
    INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.
