<?php
if (ob_get_level() === 0) {
    ob_start();
}

// Small helper function so I do not repeat the same logic everywhere.
function h(?string $value): string {
    return htmlspecialchars((string)$value, ENT_QUOTES, 'UTF-8');
}

// Small helper function so I do not repeat the same logic everywhere.
function redirect(string $url): void {
    header('Location: ' . $url);
// Stop the script here so nothing else runs after the redirect.
    exit;
}

function is_post(): bool {
// Send the result back to the code that called this function.
    return $_SERVER['REQUEST_METHOD'] === 'POST';
}

function is_valid_date(string $value): bool {
    $date = DateTime::createFromFormat('Y-m-d', $value);
    return $date !== false && $date->format('Y-m-d') === $value;
}

function flash_message(): string {
// Check this condition first so the right branch runs.
    if (!isset($_GET['msg']) || $_GET['msg'] === '') {
        return '';
    }
// Send the result back to the code that called this function.
    return '<div class="flash">' . h($_GET['msg']) . '</div>';
}

function selected($a, $b): string {
// Send the result back to the code that called this function.
    return (string)$a === (string)$b ? 'selected' : '';
}

function checked($a, $b): string {
// Send the result back to the code that called this function.
    return (string)$a === (string)$b ? 'checked' : '';
}
?>
