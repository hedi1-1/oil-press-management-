$bytes = [System.IO.File]::ReadAllBytes("c:\Users\feriel\OneDrive\Bureau\projet c++\oilll\mainwindow.ui")
$content = [System.Text.Encoding]::UTF8.GetString($bytes)

# Remove all lines with corrupted characters and replace with clean versions
$content = $content -replace 'ðŸ"¦ ', ''
$content = $content -replace 'ðŸ"‹ ', ''
$content = $content -replace 'ðŸ"ƒ ', ''
$content = $content -replace 'ðŸ"¬ ', ''
$content = $content -replace 'ðŸ"¤ ', ''
$content = $content -replace 'ðŸ"Š ', ''
$content = $content -replace 'ðŸ"œ ', ''
$content = $content -replace 'ðŸš¦ ', ''
$content = $content -replace 'â±ï¸ ', ''
$content = $content -replace 'âš ï¸ ', ''
$content = $content -replace 'ðŸ"ˆ ', ''
$content = $content -replace 'ðŸ"„ ', ''
$content = $content -replace 'ðŸ"Œ ', ''
$content = $content -replace 'ðŸ"' ', ''
$content = $content -replace 'ðŸ" ', ''
$content = $content -replace 'âœ… ', ''
$content = $content -replace 'âž• ', ''
$content = $content -replace 'âœï¸ ', ''
$content = $content -replace 'ðŸ—'ï¸ ', ''
$content = $content -replace 'ðŸ'ï¸ ', ''
$content = $content -replace 'ðŸ'¾ ', ''
$content = $content -replace 'ðŸ"… ', ''
$content = $content -replace 'âš™ï¸ ', ''
$content = $content -replace 'ðŸ"ï¸ ', ''
$content = $content -replace 'ðŸ"¥ ', ''
$content = $content -replace 'ðŸ"¡ ', ''
$content = $content -replace 'ðŸ"´ ', ''
$content = $content -replace 'â— ', ''
$content = $content -replace 'ðŸ"‰ ', ''

# Fix accented characters
$content = $content -replace 'Ã‰', 'E'
$content = $content -replace 'Ã©', 'e'
$content = $content -replace 'Ã ', 'a'
$content = $content -replace 'Ã¨', 'e'
$content = $content -replace 'Ã¢', 'a'
$content = $content -replace 'Ã®', 'i'
$content = $content -replace 'Ã´', 'o'
$content = $content -replace 'Ã»', 'u'
$content = $content -replace 'Ã§', 'c'
$content = $content -replace 'Ãª', 'e'
$content = $content -replace 'â‰¤', '<='

[System.IO.File]::WriteAllText("c:\Users\feriel\OneDrive\Bureau\projet c++\oilll\mainwindow.ui", $content, [System.Text.Encoding]::UTF8)
Write-Host "Done"
