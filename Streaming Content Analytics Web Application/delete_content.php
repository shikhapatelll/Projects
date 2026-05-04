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
    $stmt = $conn->prepare("SELECT contentid, title, contenttype FROM content WHERE contentid = ?");
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
    $action = $_POST['action'] ?? '';
// Check this condition first so the right branch runs.
    if ($action === 'cancel') {
        redirect('content.php?msg=' . urlencode('Deletion cancelled.'));
    }
// Check this condition first so the right branch runs.
    if (!$content) {
        $error = 'Content not found.';
    } elseif ($action === 'confirm_delete') {
// statement here to keep the SQL safer and cleaner.
        $stmt = $conn->prepare("DELETE FROM content WHERE contentid = ?");
        $cid = (int)$contentid;
// Match the PHP values to the placeholders before executing the query.
        $stmt->bind_param('i', $cid);
        if ($stmt->execute()) {
            $stmt->close();
// After success, send the user to the next page with a friendly message.
            redirect('content.php?msg=' . urlencode('Content deleted successfully. Related watches, ratings, and episodes were removed by foreign keys if applicable.'));
        }
        $error = 'Delete failed: ' . $stmt->error;
        $stmt->close();
    }
}
?>
<!-- Main section of this page starts here. -->
<section>
    <h2>Delete Content</h2>
    <?php if ($error): ?><div class="error"><?= h($error); ?></div><?php endif; ?>
    <?php if (!$content): ?>
        <div class="error">No valid content selected.</div>
    <?php else: ?>
        <div class="card">
            <p><strong>Warning:</strong> This will permanently delete <strong><?= h($content['title']); ?></strong> (ID <?= h($content['contentid']); ?>).</p>
            <p class="small">Because of your Assignment 2 foreign keys, related episodes, viewing events, and ratings may also be deleted automatically.</p>
<!-- Form starts here so the user can enter or update information. -->
            <form method="post" class="inline-form">
                <input type="hidden" name="contentid" value="<?= h($content['contentid']); ?>">
<!-- Input field where the user types or picks a value. -->
                <input type="hidden" name="action" value="confirm_delete">
                <button class="danger" type="submit">Yes, Delete Permanently</button>
            </form>
<!-- Form starts here so the user can enter or update information. -->
            <form method="post" class="inline-form">
                <input type="hidden" name="contentid" value="<?= h($content['contentid']); ?>">
<!-- Input field where the user types or picks a value. -->
                <input type="hidden" name="action" value="cancel">
                <button class="secondary" type="submit">Cancel</button>
            </form>
        </div>
    <?php endif; ?>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
