<?php
// Pull in a shared file here so this page can reuse common code.
require_once 'includes/db.php';
require_once 'includes/header.php';
// Open the database connection because this page needs live data.
$conn = getDB();
$error = '';
// Read the submitted/requested value first so I can validate it safely.
$contentid = $_GET['contentid'] ?? ($_POST['contentid'] ?? '');
$series = null;
// Store this in a variable so the next lines stay easier to read.
$seriesResult = $conn->query("SELECT contentid, title FROM content WHERE contenttype = 'tv_series' ORDER BY title");

if ($contentid !== '' && ctype_digit((string)$contentid)) {
// I use a prepared statement here to keep the SQL safer and cleaner.
    $checkSeries = $conn->prepare("SELECT contentid, title FROM content WHERE contentid = ? AND contenttype = 'tv_series'");
    $cid = (int)$contentid;
// Match the PHP values to the placeholders before executing the query.
    $checkSeries->bind_param('i', $cid);
    $checkSeries->execute();
// Read one row at a time into an associative array.
    $series = $checkSeries->get_result()->fetch_assoc();
    $checkSeries->close();
}

if (is_post()) {
// Read the submitted/requested value first so I can validate it safely.
    $contentid = trim($_POST['contentid'] ?? '');
    $episodenumber = trim($_POST['episodenumber'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $title = trim($_POST['title'] ?? '');
    $duration = trim($_POST['duration'] ?? '');
// Read the submitted/requested value first so I can validate it safely.
    $releasedate = trim($_POST['releasedate'] ?? '');

    if (!ctype_digit($contentid) || (int)$contentid <= 0 || !ctype_digit($episodenumber) || (int)$episodenumber <= 0 || $title === '' || !ctype_digit($duration) || (int)$duration <= 0 || !is_valid_date($releasedate)) {
// I keep one error message variable so I can show feedback neatly.
        $error = 'Please enter valid episode information.';
    } else {
        $cid = (int)$contentid;
// I use a prepared statement here to keep the SQL safer and cleaner.
        $checkSeries = $conn->prepare("SELECT contentid FROM content WHERE contentid = ? AND contenttype = 'tv_series'");
        $checkSeries->bind_param('i', $cid);
// Run the query now that everything is ready.
        $checkSeries->execute();
        $exists = $checkSeries->get_result()->fetch_assoc();
        $checkSeries->close();

// Check this condition first so the right branch runs.
        if (!$exists) {
            $error = 'That content id is not a valid TV series.';
        } else {
// I use a prepared statement here to keep the SQL safer and cleaner.
            $checkEp = $conn->prepare("SELECT contentid FROM episodes WHERE contentid = ? AND episodenumber = ?");
            $epnum = (int)$episodenumber;
// Match the PHP values to the placeholders before executing the query.
            $checkEp->bind_param('ii', $cid, $epnum);
            $checkEp->execute();
// Read one row at a time into an associative array.
            $dup = $checkEp->get_result()->fetch_assoc();
            $checkEp->close();

            if ($dup) {
// keeping one error message variable so I can show feedback neatly.
                $error = 'That episode number already exists for this series.';
            } else {
                $stmt = $conn->prepare("INSERT INTO episodes (contentid, episodenumber, title, duration, releasedate) VALUES (?, ?, ?, ?, ?)");
// Store this in a variable so the next lines stay easier to read.
                $dur = (int)$duration;
                $stmt->bind_param('iisis', $cid, $epnum, $title, $dur, $releasedate);
// Run the query now that everything is ready.
                if ($stmt->execute()) {
                    $stmt->close();
                    redirect('add_episode.php?contentid=' . urlencode($contentid) . '&msg=' . urlencode('Episode added successfully.'));
                } else {
//keeping one error message variable so I can show feedback neatly.
                    $error = 'Episode insert failed: ' . $stmt->error;
                    $stmt->close();
                }
            }
        }
    }
}
?>
<section>
<!-- Heading helps the user see what this page or section does. -->
    <h2>Add Episode to TV Series</h2>
    <?php if ($error): ?><div class="error"><?= h($error); ?></div><?php endif; ?>
    <form method="post" class="card">
        <div class="grid">
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="contentid">TV Series</label>
                <select name="contentid" id="contentid" required>
<!-- One choice inside the dropdown menu. -->
                    <option value="">Select a series</option>
                    <?php while ($row = $seriesResult->fetch_assoc()): ?>
                        <option value="<?= h($row['contentid']); ?>" <?= selected($contentid, $row['contentid']); ?>>
                            <?= h($row['title']); ?> (ID <?= h($row['contentid']); ?>)
                        </option>
                    <?php endwhile; ?>
                </select>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="episodenumber">Episode Number</label>
                <input type="number" name="episodenumber" id="episodenumber" required>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="title">Episode Title</label>
                <input type="text" name="title" id="title" required>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="duration">Duration (minutes)</label>
                <input type="number" name="duration" id="duration" required>
            </div>
            <div>
<!-- Label makes the form field easier to understand. -->
                <label for="releasedate">Release Date</label>
                <input type="date" name="releasedate" id="releasedate" required>
            </div>
        </div>
<!-- Button submits the form or triggers the main action. -->
        <button type="submit">Add Episode</button>
    </form>

    <?php if ($series): ?>
        <div class="card">
            <h3>Existing Episodes for <?= h($series['title']); ?></h3>
            <?php
// statement here to keep the SQL safer and cleaner.
            $stmt = $conn->prepare("SELECT episodenumber, title, duration, releasedate FROM episodes WHERE contentid = ? ORDER BY episodenumber");
            $cid = (int)$series['contentid'];
// Match the PHP values to the placeholders before executing the query.
            $stmt->bind_param('i', $cid);
            $stmt->execute();
// Store this in a variable so the next lines stay easier to read.
            $eps = $stmt->get_result();
            ?>
            <table>
                <tr><th>Episode #</th><th>Title</th><th>Duration</th><th>Release Date</th></tr>
                <?php while ($ep = $eps->fetch_assoc()): ?>
                    <tr>
                        <td><?= h($ep['episodenumber']); ?></td>
                        <td><?= h($ep['title']); ?></td>
                        <td><?= h($ep['duration']); ?> min</td>
                        <td><?= h($ep['releasedate']); ?></td>
                    </tr>
                <?php endwhile; ?>
            </table>
            <?php $stmt->close(); ?>
        </div>
    <?php endif; ?>
</section>
<?php closeDB($conn); require_once 'includes/footer.php'; ?>
