tell application "Music"
	if skipped date of current track is not missing value then
		set SkpDt to skipped date of current track as �class isot� as string
	else
		set SkpDt to ""
	end if
	if played date of current track is not missing value then
		set PlyDt to played date of current track as �class isot� as string
	else
		set PlyDt to ""
	end if
	get {file location, name, album, artist, genre, duration, year} of current track & player position & (player state as string) & SkpDt & PlyDt
end tell