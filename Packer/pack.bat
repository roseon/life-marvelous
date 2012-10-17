@echo off

echo .
echo ---------------------------------------------------------------------
echo  Archiving system.
echo ---------------------------------------------------------------------
echo .
MRSPacker a:100 "system.gfs" "system\*.*"
MRSPacker a:100 "custom.gfs" "custom\*.*"

echo .
echo ---------------------------------------------------------------------
echo  Archiving sfx.
echo ---------------------------------------------------------------------
echo .

MRSPacker a:100 "sfx.gfs" "sfx\*.*"

echo .
echo ---------------------------------------------------------------------
echo  Archiving interface.
echo ---------------------------------------------------------------------
echo .
MRSPacker a:100 "interface.gfs" "interface\default\*.*"
MRSPacker a:100 "interface.gfs" "interface\default\combat\*.*"
MRSPacker a:100 "interface.gfs" "interface\default\loading\*.*"
MRSPacker a:100 "interface.gfs" "interface\loadable\*.*"
MRSPacker a:100 "interface.gfs" "interface\login\*.*"
MRSPacker a:100 "interface.gfs" "interface\MonsterIllust\*.*"

echo .
echo ---------------------------------------------------------------------
echo  Archiving models.
echo ---------------------------------------------------------------------
echo .

MRSPacker a:100 "model.gfs" "model\*.*"
MRSPacker a:100 "model.gfs" "model\lo\*.*"
MRSPacker a:100 "model.gfs" "model\man\*.*"
MRSPacker a:100 "model.gfs" "model\woman\*.*"
MRSPacker a:100 "model.gfs" "model\worlditem\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\blade\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\dagger\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\grenade\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\item\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\katana\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\pistol\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\rifle\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\rocketlauncher\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\shotgun\*.*"
MRSPacker a:100 "model.gfs" "model\weapon\sword\*.*"
MRSPacker a:100 "model.gfs" "model\NPC\goblin\*.*"
MRSPacker a:100 "model.gfs" "model\NPC\golem\*.*"
MRSPacker a:100 "model.gfs" "model\NPC\kobolt\*.*"
MRSPacker a:100 "model.gfs" "model\NPC\palmpoa\*.*"
MRSPacker a:100 "model.gfs" "model\NPC\skeleton\*.*"

echo .
echo ---------------------------------------------------------------------
echo  Archiving maps. - Please wait as this may take a while!
echo ---------------------------------------------------------------------
echo .

cd Maps
fuckmaietv2
cd ..

echo .
echo ---------------------------------------------------------------------
echo  Archiving quests. - Please wait as this may take a while!
echo ---------------------------------------------------------------------
echo .

cd Quest
fuckmaietv2
cd ..

echo .
echo ---------------------------------------------------------------------
echo  Archiving sound.
echo ---------------------------------------------------------------------
echo .

MRSPacker a:100 "sound.gfs" "sound\bgm\*.*"
MRSPacker a:100 "sound.gfs" "sound\effect\*.*"
MRSPacker a:100 "sound.gfs" "sound\effect\fx2\*.*"
MRSPacker a:100 "sound.gfs" "sound\effect\nar\*.*"
MRSPacker a:100 "sound.gfs" "sound\effect\quest\goblin\*.*"
MRSPacker a:100 "sound.gfs" "sound\effect\quest\golem\*.*"
MRSPacker a:100 "sound.gfs" "sound\effect\quest\lizard\*.*"
MRSPacker a:100 "sound.gfs" "sound\effect\quest\skeleton\*.*"


echo .
echo ---------------------------------------------------------------------
echo  Merging indexs
echo ---------------------------------------------------------------------
echo .
MRSPacker m "fileindex.msf" "*.index"
MRSPacker m "fileindex.msf" "Maps\*.index"
MRSPACKER m "fileindex.msf" "Quest\*.index"

del /Q "custom.gfs" y
del /Q "Quest\*.index" y
del /Q "*.index" y
del /Q "Maps\*.index" y
