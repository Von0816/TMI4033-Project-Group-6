#   written by Boonlert Randy (74297) for Group 6 TMI4033 Project

from flask import Flask, request, render_template, flash
from timeSVD import svd 
import secrets 
import re

n_svd = svd()

app = Flask(__name__)
app.secret_key = secrets.token_urlsafe(16)

movie_dict = {'1': 'Dinosaur Planet',
              '2': 'Isle of Man TT 2004 Review',
              '3': 'Character',
              '4': 'Paula Abdul\'s Get Up & Dance',
              '5': 'The Rise and Fall of ECW',
              '6': 'Sick',
              '7': '8 Man',
              '8': 'What the #$*! Do We Know!?',
              '9': 'Class of Nuke \'Em High 2',
              '10': 'Fighter'
              } 

p_score = {}


@app.route('/', methods=["GET", 'POST'])
def index():
    if request.method == 'POST':
        u_id = request.form['u_id']
        m_id = request.form['m_id']
        t = request.form['date']
        if not u_id:
            flash('Please enter the user ID', 'error')
        elif re.search('[a-z]+', u_id):
            flash('Invalid user ID', 'error')
        elif int(u_id) < 1 or int(u_id) > 2649430:
            flash('Invalid user ID', 'error')
        elif not m_id:
            flash('Please enter the movie ID', 'error')
        elif movie_dict.get(str(request.form['m_id']), 0) == 0 or re.search('[a-z]+', m_id):
            flash('Invalid movie ID', 'error')
        elif not t:
            flash('Please enter the rating date', 'error')
        else:
            p_score['user_id'] = u_id
            p_score['movie'] = movie_dict[m_id] 
            p_score['date'] = t
            p_score['rating'] = round(n_svd.predictScore(u_id, m_id, t), 2)
            flash(p_score, "result")
    return render_template('index.html')

