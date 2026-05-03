# Changelog

## [1.1.0](https://github.com/FinOrr/embedded-nmea-0183/compare/v1.0.0...v1.1.0) (2026-05-03)


### Features

* Add ARC sentence support. ([967fc5f](https://github.com/FinOrr/embedded-nmea-0183/commit/967fc5fe59f43b21f568571a591e1b84ad3e2fc3))
* Add ARC support to config.h ([eb87fae](https://github.com/FinOrr/embedded-nmea-0183/commit/eb87fae0bd895395dbfe815e65146ff63739c7d6))
* Add checksum and address fields to sentence structures. ([967fc5f](https://github.com/FinOrr/embedded-nmea-0183/commit/967fc5fe59f43b21f568571a591e1b84ad3e2fc3))
* Add enum types for ACA parameters ([967fc5f](https://github.com/FinOrr/embedded-nmea-0183/commit/967fc5fe59f43b21f568571a591e1b84ad3e2fc3))
* Add missing modules ([352fc0d](https://github.com/FinOrr/embedded-nmea-0183/commit/352fc0dcfb6735c20b49d75a8bc4a45aa9d78258))
* Add safety module, radar module ([05c2238](https://github.com/FinOrr/embedded-nmea-0183/commit/05c22389fc4deb1b1c59b27d454666222ef72bea))
* Added ALR sentence ([0ba45cc](https://github.com/FinOrr/embedded-nmea-0183/commit/0ba45ccae868b179e6771095cc9f5dfef6b64818))
* Added APB sentence. ([6793fc8](https://github.com/FinOrr/embedded-nmea-0183/commit/6793fc8922161310e494608d88b22c27ef8f21e9))
* Added GNSS, sensor, heading, navigation, waypoint, AIS modules ([c7663fe](https://github.com/FinOrr/embedded-nmea-0183/commit/c7663fe38730733f13d90f5933599e8dcba5f44c))
* **comm:** Added nmea communications module ([e73049f](https://github.com/FinOrr/embedded-nmea-0183/commit/e73049f4697eec0e42bfedcb7ff7ff042686935b))
* **misc:** Added nmea miscellaneous module header, for functions I don't know how else to classify ([7e66366](https://github.com/FinOrr/embedded-nmea-0183/commit/7e66366bf20bb7f05d1c8dd7cf942e5e4131a4e6))
* Nmea attitude module ([31e1acb](https://github.com/FinOrr/embedded-nmea-0183/commit/31e1acb8c01c58e8a8d1c9ab0b9f22a4b84e7972))
* **nmea_parser.h:** Add new header-only design for the parser ([10dc40b](https://github.com/FinOrr/embedded-nmea-0183/commit/10dc40b315da48f02ef67668022c224b4e3f4186))
* Remove old header files for the original design, move to header-only implementation ([5c31245](https://github.com/FinOrr/embedded-nmea-0183/commit/5c31245ab3e91c8b3ddaaf2241f8bc9f4e8bc86a))
* **system:** Added nmea system module ([28a979c](https://github.com/FinOrr/embedded-nmea-0183/commit/28a979c2ca2a1b97b0ff6c579ba35d34595ba60e))


### Bug Fixes

* **CMakePresets.json:** Removed unused reference to mutation tests that caused presets to fail ([b1ddee2](https://github.com/FinOrr/embedded-nmea-0183/commit/b1ddee2d9ae34d6816975514986cd9344ddc2786))
* Config ALF enabled typo ([f1f1d84](https://github.com/FinOrr/embedded-nmea-0183/commit/f1f1d844252366342ebbf89bcc3c6f3d9304751d))
* **devcontainer-license:** Included the license from the Philips AMP Devcontainer repo to properly credit them ([be76ffc](https://github.com/FinOrr/embedded-nmea-0183/commit/be76ffc7c24eff5d88af2f1716736db2d6366870))
* Fix for failing tests ([de24093](https://github.com/FinOrr/embedded-nmea-0183/commit/de24093242bedcd2a06451529d6b8d7fcf56a5e3))
* Fix for failing tests ([d27ca94](https://github.com/FinOrr/embedded-nmea-0183/commit/d27ca94460e8715cc3568d582a6e2b15a8f671c6))
* Remove incorrect status fields. ([967fc5f](https://github.com/FinOrr/embedded-nmea-0183/commit/967fc5fe59f43b21f568571a591e1b84ad3e2fc3))
* Supported sentences link no longer broken ([6948a54](https://github.com/FinOrr/embedded-nmea-0183/commit/6948a5431842c1610a9667fedc7210f67449092a))


### Chores

* Consistent formatting ([967fc5f](https://github.com/FinOrr/embedded-nmea-0183/commit/967fc5fe59f43b21f568571a591e1b84ad3e2fc3))
* Consistent formatting ([6793fc8](https://github.com/FinOrr/embedded-nmea-0183/commit/6793fc8922161310e494608d88b22c27ef8f21e9))
* **main:** Release 1.0.0 ([80b78a6](https://github.com/FinOrr/embedded-nmea-0183/commit/80b78a608f113b9db262b45d0bb3c4d47ad41667))
* **main:** Release 1.0.0 ([f8ef492](https://github.com/FinOrr/embedded-nmea-0183/commit/f8ef492219ca610c8ebff2aa5a2f15b1789a41da))
* **release:** Release v2.0.0 ([4491534](https://github.com/FinOrr/embedded-nmea-0183/commit/4491534c0674968abd2b2ae715a41eaad6ef1b99))
* Sentence enable macros prefixed with "CFG_". ([0ba45cc](https://github.com/FinOrr/embedded-nmea-0183/commit/0ba45ccae868b179e6771095cc9f5dfef6b64818))

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
