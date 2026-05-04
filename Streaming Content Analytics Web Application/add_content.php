<?php
// Pull in a shared file here so this page can reuse common code.
require_once 'includes/db.php';
require_once 'includes/header.php';
// Open the database connection because this page needs live data.
$conn = getDB();
$error = '';

// Only run this block after the user submits the form.
if (is_post()) {
    $contentid = trim($_POST['contentid'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $title = trim($_POST['title'] ?? '');
    $releaseyear = trim($_POST['releaseyear'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $agerating = trim($_POST['agerating'] ?? '');
    $language = trim($_POST['language'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $contenttype = $_POST['contenttype'] ?? 'movie';
    $duration = trim($_POST['duration'] ?? '');

// Check this condition first so the right branch runs.
    if (!ctype_digit($contentid) || (int)$contentid <= 0 || $title === '' || !ctype_digit($releaseyear) || (int)$releaseyear <= 0 || $agerating === '' || $language === '') {
        $error = 'Please fill in all required fields correctly.';
    } elseif (!in_array($contenttype, ['movie', 'tv_series'], true)) {
// I keep one error message variable so I can show feedback neatly.
        $error = 'Invalid content type.';
    } elseif ($contenttype === 'movie' && (!ctype_digit($duration) || (int)$duration <= 0)) {
        $error = 'Movie duration must be a whole number of minutes.';
    } else {
// statement here to keep the SQL safer and cleaner.
        $check = $conn->prepare("SELECT contentid FROM content WHERE contentid = ?");
        $check->bind_param('i', $contentid);
// Run the query now that everything is ready.
        $check->execute();
        $exists = $check->get_result()->fetch_assoc();
        $check->close();

// Check this condition first so the right branch runs.
        if ($exists) {
            $error = 'That content identifier already exists. Duplicate IDs are not allowed.';
        } else {
// Check this condition first so the right branch runs.
            if ($contenttype === 'movie') {
                $stmt = $conn->prepare("INSERT INTO content (contentid, title, releaseyear, agerating, language, contenttype, duration) VALUES (?, ?, ?, ?, ?, ?, ?)");
// Store this in a variable so the next lines stay easier to read.
                $dur = (int)$duration;
                $ry = (int)$releaseyear;
// Store this in a variable so the next lines stay easier to read.
                $cid = (int)$contentid;
                $stmt->bind_param('isisssi', $cid, $title, $ry, $agerating, $language, $contenttype, $dur);
            } else {
// statement here to keep the SQL safer and cleaner.
                $stmt = $conn->prepare("INSERT INTO content (contentid, title, releaseyear, agerating, language, contenttype, duration) VALUES (?, ?, ?, ?, ?, ?, NULL)");
                $ry = (int)$releaseyear;
// Store this in a variable so the next lines stay easier to read.
                $cid = (int)$contentid;
                $stmt->bind_param('isisss', $cid, $title, $ry, $agerating, $language, $contenttype);
            }

// Run the query now that everything is ready.
            if ($stmt->execute()) {
                $stmt->close();
                if ($contenttype === 'tv_series') {
// After success, send the user to the next page with a friendly message.
                    redirect('add_episode.php?contentid=' . urlencode($contentid) . '&msg=' . urlencode('TV series added successfully. Now add one or more episodes.'));
                }
                redirect('content.php?msg=' . urlencode('Content added successfully.'));
            } else {
//keeping one error message variable so I can show feedback neatly.
                $error = 'Insert failed: ' . $stmt->error;
                $stmt->close();
            }
        }
    }
}
?>
<section>
<!-- Heading helps the user see what this page or section does. -->
    <h2>Add Content</h2>
    <?php if ($error): ?><div class="error"><?= h($error); ?></div><?php endif; ?>
    <form method="post" class="card">
        <div class="grid">
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="contentid">Content ID</label>
                <input type="number" name="contentid" id="contentid" required>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="title">Title</label>
                <input type="text" name="title" id="title" required>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="releaseyear">Release Year</label>
                <input type="number" name="releaseyear" id="releaseyear" required>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="agerating">Age Rating</label>
                <input type="text" name="agerating" id="agerating" required>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="language">Language</label>
                <input type="text" name="language" id="language" required>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="contenttype">Content Type</label>
                <select name="contenttype" id="contenttype" onchange="document.getElementById('movieDurationWrap').style.display = this.value === 'movie' ? 'block' : 'none';">
<!-- One choice inside the dropdown menu. -->
                    <option value="movie">Movie</option>
                    <option value="tv_series">TV Series</option>
                </select>
            </div>
        </div>
        <div id="movieDurationWrap">
<!-- Label makes the form field easier to understand. -->
            <label for="duration">Movie Duration (minutes)</label>
            <input type="number" name="duration" id="duration">
        </div>
<!-- Button submits the form or triggers the main action. -->
        <button type="submit">Add Content</button>
    </form>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
