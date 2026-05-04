I built a PHP/MySQL streaming content analytics web application that supports content browsing, CRUD operations, ratings, viewing history filters, subscription plan updates, and a clear user interface.

Folder contents:
- mainmenu.php
- content.php
- content_details.php
- add_content.php
- add_episode.php
- update_content.php
- delete_content.php
- users.php
- ratings.php
- plans.php
- style.css
- includes/config.php
- includes/db.php
- includes/functions.php
- includes/header.php
- includes/footer.php


Notes:
- solution uses class schema:
  subscription_plans, users, content, episodes, profile, watches, rates
- Content ID is never editable during updates.
- Ratings are updated if the user already rated that content.
- Delete content asks for confirmation before deletion.
