EGW3200/SP-TGL board needs a project property settings change.

Open "Sema4" solution in visual studio. Open "SemaEapi" Library project property. Go to "Configuration Properties-> C/C++ -> Preprocessor" and select "Preprocessor Definitions" option. Add "EGW3200" at the end of "Preprocessor Definitions" and save the project property.

This setting is already enabled in commit "fb2dd17d34d833ff0bf3c0e3c5562fcd4f1ca654"