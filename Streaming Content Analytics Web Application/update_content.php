<?php
// Pull in a shared file here so this page can reuse common code.
require_once 'includes/db.php';
require_once 'includes/header.php';
// Open the database connection because this page needs live data.
$conn = getDB();
$error = '';
// Read the submitted/requested value first so I can validate it safely.
$contentid = $_GET['contentid'] ?? ($_POST['contentid'] ?? '');
$content = null;

// Check this condition first so the right branch runs.
if ($contentid !== '' && ctype_digit((string)$contentid)) {
    $stmt = $conn->prepare("SELECT contentid, title, agerating, language FROM content WHERE contentid = ?");
// Store this in a variable so the next lines stay easier to read.
    $cid = (int)$contentid;
    $stmt->bind_param('i', $cid);
// Run the query now that everything is ready.
    $stmt->execute();
    $content = $stmt->get_result()->fetch_assoc();
    $stmt->close();
}

// Only run this block after the user submits the form.
if (is_post()) {
    $title = trim($_POST['title'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $agerating = trim($_POST['agerating'] ?? '');
    $language = trim($_POST['language'] ?? '');

// Check this condition first so the right branch runs.
    if (!$content) {
        $error = 'Content not found.';
    } elseif ($title === '' || $agerating === '' || $language === '') {
// keeping one error message variable so I can show feedback neatly.
        $error = 'Please fill in all editable fields.';
    } else {
        $stmt = $conn->prepare("UPDATE content SET title = ?, agerating = ?, language = ? WHERE contentid = ?");
// Store this in a variable so the next lines stay easier to read.
        $cid = (int)$contentid;
        $stmt->bind_param('sssi', $title, $agerating, $language, $cid);
// Run the query now that everything is ready.
        if ($stmt->execute()) {
            $stmt->close();
            redirect('content.php?msg=' . urlencode('Content updated successfully. Content ID was not modified.'));
        }
// keeping one error message variable so I can show feedback neatly.
        $error = 'Update failed: ' . $stmt->error;
        $stmt->close();
    }
}
?>
<section>
<!-- Heading helps the user see what this page or section does. -->
    <h2>Update Content</h2>
    <?php if ($error): ?><div class="error"><?= h($error); ?></div><?php endif; ?>
    <?php if (!$content): ?>
        <div class="error">Please choose a valid content item from the content page.</div>
    <?php else: ?>
        <form method="post" class="card">
<!-- Input field where the user types or picks a value. -->
            <input type="hidden" name="contentid" value="<?= h($content['contentid']); ?>">
            <label>Content ID (cannot be changed)</label>
<!-- Input field where the user types or picks a value. -->
            <input type="text" value="<?= h($content['contentid']); ?>" disabled>
            <label for="title">Title</label>
<!-- Input field where the user types or picks a value. -->
            <input type="text" name="title" id="title" value="<?= h($content['title']); ?>" required>
            <label for="agerating">Age Rating</label>
<!-- Input field where the user types or picks a value. -->
            <input type="text" name="agerating" id="agerating" value="<?= h($content['agerating']); ?>" required>
            <label for="language">Language</label>
<!-- Input field where the user types or picks a value. -->
            <input type="text" name="language" id="language" value="<?= h($content['language']); ?>" required>
            <button type="submit">Update Content</button>
        </form>
    <?php endif; ?>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
