$bytes = [System.IO.File]::ReadAllBytes('assets/images/many/Many-action.tim')
$tag = [BitConverter]::ToUInt32($bytes, 0)
$mode = [BitConverter]::ToUInt32($bytes, 4)
$offset = 8
Write-Output ("MANYACT TIM tag: 0x{0:X}, mode: 0x{1:X}" -f $tag, $mode)
if ($mode -band 8) {
    $clut_len = [BitConverter]::ToUInt32($bytes, $offset)
    $clut_x = [BitConverter]::ToUInt16($bytes, $offset + 4)
    $clut_y = [BitConverter]::ToUInt16($bytes, $offset + 6)
    $clut_w = [BitConverter]::ToUInt16($bytes, $offset + 8)
    $clut_h = [BitConverter]::ToUInt16($bytes, $offset + 10)
    Write-Output ("CLUT: len={0}, x={1}, y={2}, w={3}, h={4}" -f $clut_len, $clut_x, $clut_y, $clut_w, $clut_h)
    $offset += $clut_len
}
$img_len = [BitConverter]::ToUInt32($bytes, $offset)
$img_x = [BitConverter]::ToUInt16($bytes, $offset + 4)
$img_y = [BitConverter]::ToUInt16($bytes, $offset + 6)
$img_w = [BitConverter]::ToUInt16($bytes, $offset + 8)
$img_h = [BitConverter]::ToUInt16($bytes, $offset + 10)
Write-Output ("IMG: len={0}, x={1}, y={2}, w={3}, h={4}" -f $img_len, $img_x, $img_y, $img_w, $img_h)
