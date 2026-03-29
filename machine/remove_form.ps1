[xml]$doc = Get-Content 'C:\Users\PC\Desktop\A M A L\machine\machine.ui'
$node = $doc.SelectSingleNode("//widget[@name='widgetFormulaireParcMachines']")
if ($node) {
    $node.ParentNode.RemoveChild($node) > $null
    $doc.Save('C:\Users\PC\Desktop\A M A L\machine\machine.ui')
    Write-Host "Successfully removed widgetFormulaireParcMachines"
} else {
    Write-Host "Node not found"
}
