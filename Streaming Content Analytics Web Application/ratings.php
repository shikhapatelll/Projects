<?php
// Pull in a shared file here so this page can reuse common code.
require_once 'includes/db.php';
require_once 'includes/header.php';
// Open the database connection because this page needs live data.
$conn = getDB();
$error = '';

// Only run this block after the user submits the form.
if (is_post()) {
    $userid = trim($_POST['userid'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $contentid = trim($_POST['contentid'] ?? '');
    $score = trim($_POST['score'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $ratingDate = trim($_POST['rating_date'] ?? '');

    if (!ctype_digit($userid) || (int)$userid <= 0 || !ctype_digit($contentid) || (int)$contentid <= 0 || !ctype_digit($score) || (int)$score < 1 || (int)$score > 5 || !is_valid_date($ratingDate)) {
// I keep one error message variable so I can show feedback neatly.
        $error = 'Please enter a valid user, content item, score from 1 to 5, and date.';
    } else {
        $uid = (int)$userid;
// Store this in a variable so the next lines stay easier to read.
        $cid = (int)$contentid;
        $s = (int)$score;
        $entityCheck = $conn->prepare("SELECT
                EXISTS(SELECT 1 FROM users WHERE userid = ?) AS user_exists,
                EXISTS(SELECT 1 FROM content WHERE contentid = ?) AS content_exists");
        $entityCheck->bind_param('ii', $uid, $cid);
        $entityCheck->execute();
        $entityStatus = $entityCheck->get_result()->fetch_assoc();
        $entityCheck->close();

        if (!$entityStatus['user_exists'] || !$entityStatus['content_exists']) {
            $error = 'Selected user or content item was not found.';
        } else {
// I use a prepared statement here to keep the SQL safer and cleaner.
            $check = $conn->prepare("SELECT userid FROM rates WHERE userid = ? AND contentid = ?");
        $check->bind_param('ii', $uid, $cid);
// Run the query now that everything is ready.
        $check->execute();
        $exists = $check->get_result()->fetch_assoc();
        $check->close();

// Check this condition first so the right branch runs.
        if ($exists) {
            $stmt = $conn->prepare("UPDATE rates SET score = ?, date = ? WHERE userid = ? AND contentid = ?");
// Match the PHP values to the placeholders before executing the query.
            $stmt->bind_param('isii', $s, $ratingDate, $uid, $cid);
            $message = 'Existing rating updated successfully.';
        } else {
// statement here to keep the SQL safer and cleaner.
            $stmt = $conn->prepare("INSERT INTO rates (userid, contentid, score, date) VALUES (?, ?, ?, ?)");
            $stmt->bind_param('iiis', $uid, $cid, $s, $ratingDate);
// Store this in a variable so the next lines stay easier to read.
            $message = 'New rating added successfully.';
        }

            if ($stmt->execute()) {
                $stmt->close();
// After success, send the user to the next page with a friendly message.
                redirect('ratings.php?msg=' . urlencode($message));
            }
            $error = 'Rating operation failed: ' . $stmt->error;
            $stmt->close();
        }
    }
}

// Store this in a variable so the next lines stay easier to read.
$users = $conn->query("SELECT userid, name FROM users ORDER BY name");
$content = $conn->query("SELECT contentid, title FROM content ORDER BY title");
// Store this in a variable so the next lines stay easier to read.
$ratingsList = $conn->query(
    "SELECT u.name AS user_name, c.title AS content_title, r.score, r.date
     FROM rates r
     JOIN users u ON r.userid = u.userid
     JOIN content c ON r.contentid = c.contentid
     ORDER BY c.title, u.name"
);
?>
<section>
<!-- Heading helps the user see what this page or section does. -->
    <h2>Ratings</h2>
    <?php if ($error): ?><div class="error"><?= h($error); ?></div><?php endif; ?>
    <div class="grid">
        <div class="card">
            <h3>Add or Update Rating</h3>
            <p class="small">If a user already rated a content item, this form updates the old rating instead of inserting a duplicate.</p>
<!-- Form starts here so the user can enter or update information. -->
            <form method="post">
                <label for="userid">User</label>
<!-- Dropdown keeps the choices controlled and easy to select. -->
                <select name="userid" id="userid" required>
                    <option value="">Select a user</option>
                    <?php while ($u = $users->fetch_assoc()): ?>
<!-- One choice inside the dropdown menu. -->
                        <option value="<?= h($u['userid']); ?>"><?= h($u['name']); ?></option>
                    <?php endwhile; ?>
                </select>
                <label for="contentid">Content</label>
<!-- Dropdown keeps the choices controlled and easy to select. -->
                <select name="contentid" id="contentid" required>
                    <option value="">Select content</option>
                    <?php while ($c = $content->fetch_assoc()): ?>
<!-- One choice inside the dropdown menu. -->
                        <option value="<?= h($c['contentid']); ?>"><?= h($c['title']); ?></option>
                    <?php endwhile; ?>
                </select>
                <label for="score">Score (1 to 5)</label>
<!-- Input field where the user types or picks a value. -->
                <input type="number" name="score" id="score" min="1" max="5" required>
                <label for="rating_date">Rating Date</label>
<!-- Input field where the user types or picks a value. -->
                <input type="date" name="rating_date" id="rating_date" required>
                <button type="submit">Submit Rating</button>
            </form>
        </div>
        <div class="card">
<!-- Heading helps the user see what this page or section does. -->
            <h3>Current Ratings</h3>
            <table>
                <tr><th>User</th><th>Content</th><th>Score</th><th>Date</th></tr>
                <?php while ($r = $ratingsList->fetch_assoc()): ?>
                    <tr>
                        <td><?= h($r['user_name']); ?></td>
                        <td><?= h($r['content_title']); ?></td>
                        <td><?= h($r['score']); ?></td>
                        <td><?= h($r['date']); ?></td>
                    </tr>
                <?php endwhile; ?>
            </table>
        </div>
    </div>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
