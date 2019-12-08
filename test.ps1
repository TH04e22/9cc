function test-case() {
    $expected = $args[0]
    $input = $args[1]

    ./9cc $input | Out-File 'tmp.s' -Encoding ascii

    gcc -o tmp tmp.s
    ./tmp
    $actual = $LastExitCode

    if( $actual -eq $expected ) {
        Write-Output "$input => $actual"
    } else {
        Write-Output "$input = > $expected expected, but got $actual"
        exit 1
    }
}

make
test-case 63 'a = 76; b = 60; c = 54; d = (a + b + c)/3;'
test-case 14 'a = 3; b = 5 * 6 - 8; a + b / 2;'

Write-Output OK
