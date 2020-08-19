(with-current-buffer (find-file-noselect "./README.org")
  (require 'org)
  (org-md-export-to-markdown))
