# Changelog

## 1.0.0 (2025-12-14)


### Features

* Add ARC sentence support. ([4892cc3](https://github.com/FinOrr/embedded-nmea-0183/commit/4892cc31e8159456a7354af7c31b2d6d1ba23c4d))
* Add ARC support to config.h ([c5c1e1b](https://github.com/FinOrr/embedded-nmea-0183/commit/c5c1e1bece9a1313b1a928dc00b4bca28d400f28))
* Add checksum and address fields to sentence structures. ([4892cc3](https://github.com/FinOrr/embedded-nmea-0183/commit/4892cc31e8159456a7354af7c31b2d6d1ba23c4d))
* Add enum types for ACA parameters ([4892cc3](https://github.com/FinOrr/embedded-nmea-0183/commit/4892cc31e8159456a7354af7c31b2d6d1ba23c4d))
* Add missing modules ([fa12010](https://github.com/FinOrr/embedded-nmea-0183/commit/fa120105a41a81c5915acc90680607dc350d205f))
* Add safety module, radar module ([89b00dd](https://github.com/FinOrr/embedded-nmea-0183/commit/89b00dd7b1ce9a42158cd459803af575ad531a61))
* Added ALR sentence ([ce9b100](https://github.com/FinOrr/embedded-nmea-0183/commit/ce9b100b0374cf575b8d9a772f5cff4a602465ce))
* Added APB sentence. ([1a80997](https://github.com/FinOrr/embedded-nmea-0183/commit/1a80997b0e84cf812a25c72b82ce50a27bd2fbe9))
* Added GNSS, sensor, heading, navigation, waypoint, AIS modules ([7f49fe4](https://github.com/FinOrr/embedded-nmea-0183/commit/7f49fe405e4899f1baa707153609b6bf792f1345))
* **comm:** Added nmea communications module ([d86164f](https://github.com/FinOrr/embedded-nmea-0183/commit/d86164fff985c94d3cb80e2c8bdc2ac3d02e5cfa))
* **misc:** Added nmea miscellaneous module header, for functions I don't know how else to classify ([0f5d271](https://github.com/FinOrr/embedded-nmea-0183/commit/0f5d27161cdaa4d3136fcc6408e56e22b7e185e2))
* Nmea attitude module ([3b408c1](https://github.com/FinOrr/embedded-nmea-0183/commit/3b408c180ea09587a494950d355bf40efc5739f3))
* **nmea_parser.h:** Add new header-only design for the parser ([02ea0fc](https://github.com/FinOrr/embedded-nmea-0183/commit/02ea0fc45a494f683297520c65b6731d94352cff))
* Remove old header files for the original design, move to header-only implementation ([2ec5dbc](https://github.com/FinOrr/embedded-nmea-0183/commit/2ec5dbc402fcdefc5def4e7fe7aaf2b11732e52f))
* **system:** Added nmea system module ([11af78d](https://github.com/FinOrr/embedded-nmea-0183/commit/11af78d4f1fa72baef58a994f53c8f3a92301955))


### Bug Fixes

* **CMakePresets.json:** Removed unused reference to mutation tests that caused presets to fail ([12ab172](https://github.com/FinOrr/embedded-nmea-0183/commit/12ab172d48845b13ff1cd2d280ea8ce68d8f07fd))
* Config ALF enabled typo ([d8537de](https://github.com/FinOrr/embedded-nmea-0183/commit/d8537de53f4c76ba9ea3c33bd460a7419c339be0))
* **devcontainer-license:** Included the license from the Philips AMP Devcontainer repo to properly credit them ([ceecd60](https://github.com/FinOrr/embedded-nmea-0183/commit/ceecd6059c993328c2dc3cb571dd5613918c07d1))
* Fix for failing tests ([ff7bd46](https://github.com/FinOrr/embedded-nmea-0183/commit/ff7bd469f5ca2404173dc8facc8db41a3a72e4fa))
* Fix for failing tests ([c9d029c](https://github.com/FinOrr/embedded-nmea-0183/commit/c9d029c39cc653d978a6cf7c816ceb42cf7d76e4))
* Remove incorrect status fields. ([4892cc3](https://github.com/FinOrr/embedded-nmea-0183/commit/4892cc31e8159456a7354af7c31b2d6d1ba23c4d))
* Supported sentences link no longer broken ([45dd073](https://github.com/FinOrr/embedded-nmea-0183/commit/45dd07359743ad08c7aef1a26ebba602a14df922))


### Chores

* Consistent formatting ([4892cc3](https://github.com/FinOrr/embedded-nmea-0183/commit/4892cc31e8159456a7354af7c31b2d6d1ba23c4d))
* Consistent formatting ([1a80997](https://github.com/FinOrr/embedded-nmea-0183/commit/1a80997b0e84cf812a25c72b82ce50a27bd2fbe9))
* **release:** Release v2.0.0 ([cf1db0d](https://github.com/FinOrr/embedded-nmea-0183/commit/cf1db0dc352ce60ff211e24830bc9904020a43e8))
* Sentence enable macros prefixed with "CFG_". ([ce9b100](https://github.com/FinOrr/embedded-nmea-0183/commit/ce9b100b0374cf575b8d9a772f5cff4a602465ce))
