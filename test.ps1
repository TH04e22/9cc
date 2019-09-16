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

test-case 21 '5+20-4'
test-case 47 '5+6*7'
test-case 4 '(3+5)/2'
test-case -8 '-(3+5)'
test-case -15 '-3*+5'
Write-Output OK
